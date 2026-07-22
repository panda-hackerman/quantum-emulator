//
// Created by Eli Michaud on 6/5/2026.
//

#include "application.h"

#include <glfw3webgpu.h>
#include <imgui.h>

#include <iostream>

#include "../resources/editorconfig_circuit.h"
#include "../resources/editorconfig_handler.h"
#include "../settings_constants.h"
#include "../theme.h"
#include "../util/device_adapter_util.h"
#include "../util/wgpu_string_view_util.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_wgpu.h"
#include "imgui_internal.h"

bool Application::Init() {

  // GLFW SETUP
  if (!glfwInit()) {
    std::cerr << "Couldn't initialize GLFW: Init function failed..." << std::endl;
    return false;
  }

  // Create window
  if (!window_.Init()) {
    std::cerr << "Couldn't create GLFW window... " << std::endl;
    return false;
  }

  // CREATE WGPU INSTANCE
#ifdef WEBGPU_BACKEND_EMSCRIPTEN
  instance_ = wgpuCreateInstance(nullptr);
#else
  constexpr WGPUInstanceDescriptor instance_desc = {.nextInChain = nullptr};
  instance_ = wgpuCreateInstance(&instance_desc);
#endif

  // GET SURFACE
  surface_ = glfwCreateWindowWGPUSurface(instance_, window_.GetNative());

  // REQUEST ADAPTER
  std::cout << "Requesting Adapter..." << '\n';

  const WGPURequestAdapterOptions adapter_options = {
      .nextInChain = nullptr,
      .featureLevel = WGPUFeatureLevel_Core,
      .compatibleSurface = surface_,
  };

  WGPUAdapter adapter = RequestAdapterSync(instance_, &adapter_options);

  std::cout << "Got Adapter: " << adapter << '\n';

  InspectAdapter(adapter); // DEBUG

  { // GET DEVICE
    std::cout << "Requesting Device..." << '\n';

    constexpr WGPUDeviceDescriptor device_desc = {
        .nextInChain = nullptr,
        .label = "The Device"_w,
        .requiredFeatureCount = 0,
        .requiredFeatures = nullptr,
        .requiredLimits = &kDeviceRequiredLimits,
        .defaultQueue = {.nextInChain = nullptr, .label = "The Default Queue"_w},
        .deviceLostCallbackInfo = {.nextInChain = nullptr,
                                   .mode = WGPUCallbackMode_AllowProcessEvents,
                                   .callback = OnDeviceLostCallback},
        .uncapturedErrorCallbackInfo = {.nextInChain = nullptr, .callback = OnDeviceErrorCallback},
    };

    device_ = RequestDeviceSync(instance_, adapter, &device_desc);

    std::cout << "Got Device: " << device_ << '\n';
  }

  InspectDevice(device_);

  // GET QUEUE
  std::cout << "\nGetting Queue: " << '\n';
  queue_ = wgpuDeviceGetQueue(device_);
  std::cout << "Got Queue: " << queue_ << '\n';

  { // CONFIGURE SURFACE
    std::cout << "\nConfiguring Surface: " << '\n';
    WGPUSurfaceCapabilities capabilities = {};
    wgpuSurfaceGetCapabilities(surface_, adapter, &capabilities);

    std::cout << "Got surface capabilities..." << '\n';
    preferred_format_ = capabilities.formats[0];

    const WGPUSurfaceConfiguration surface_config = BuildSurfaceConfig(window_);

    wgpuSurfaceConfigure(surface_, &surface_config);
    wgpuSurfaceCapabilitiesFreeMembers(capabilities);
  }

  std::cout << "Configured surface successfully! " << '\n';

  // Release stuff we don't need anymore
  wgpuAdapterRelease(adapter);

  // INIT GUI
  std::cout << "\nInitializing ImGui..." << '\n';

  window_manager_.Init();

  ImGui_ImplGlfw_InitForOther(window_.GetNative(), true);

  ImGui_ImplWGPU_InitInfo init_info;
  init_info.Device = device_;
  init_info.RenderTargetFormat = preferred_format_;

  ImGui_ImplWGPU_Init(&init_info);

  texture_manager_.LoadAllTextures(device_);

  // Now running... B)
  std::cout << "\nFinished Initialization! " << std::endl;
  is_running_ = true;
  return true;
}

void Application::BeginFrame() {
  ImGui_ImplWGPU_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();
}

void Application::DrawFrame() {
  window_manager_.DrawWindows();
}

void Application::EndFrame() {
  ImGui::Render();

  // Get the next target view
  WGPUTextureView target_view = GetNextTextureView();
  if (!target_view) return;

  // GET ENCODER
  const WGPUCommandEncoderDescriptor encoder_desc = {
      .nextInChain = nullptr,
      .label = "The Command Encoder"_w,
  };

  WGPUCommandEncoder encoder = wgpuDeviceCreateCommandEncoder(device_, &encoder_desc);

  // GET RENDER PASS
  const WGPURenderPassColorAttachment color_attachment = {
      .view = target_view,
#ifndef WEBGPU_BACKEND_WGPU
      .depthSlice = WGPU_DEPTH_SLICE_UNDEFINED,
#endif
      .resolveTarget = nullptr,
      .loadOp = WGPULoadOp_Clear,
      .storeOp = WGPUStoreOp_Store,
      .clearValue = theme::kWindowClearColor,
  };

  const WGPURenderPassDescriptor render_pass_desc = {
      .colorAttachmentCount = 1,
      .colorAttachments = &color_attachment,
      .depthStencilAttachment = nullptr,
      .timestampWrites = nullptr,
  };

  // CREATE RENDER PASS
  WGPURenderPassEncoder render_pass = wgpuCommandEncoderBeginRenderPass(encoder, &render_pass_desc);

  ImGui_ImplWGPU_RenderDrawData(ImGui::GetDrawData(), render_pass);

  wgpuRenderPassEncoderEnd(render_pass);
  wgpuRenderPassEncoderRelease(render_pass);

  // ENCODE AND SUBMIT RENDER PASS
  constexpr WGPUCommandBufferDescriptor cmd_buffer_desc = {
      .nextInChain = nullptr,
      .label = "The Command Buffer"_w,
  };

  WGPUCommandBuffer command = wgpuCommandEncoderFinish(encoder, &cmd_buffer_desc);
  wgpuCommandEncoderRelease(encoder);

  wgpuQueueSubmit(queue_, 1, &command);
  wgpuCommandBufferRelease(command);

  // RELEASING STUFF
  wgpuTextureViewRelease(target_view);

  // PRESENTING STUFF
#ifndef __EMSCRIPTEN__
  wgpuSurfacePresent(surface_);
#endif
}

void Application::Tick() {
  glfwPollEvents();
  wgpuInstanceProcessEvents(instance_);

  BeginFrame();
  DrawFrame();
  EndFrame();
}

bool Application::ShouldContinue() const noexcept {
  return !glfwWindowShouldClose(window_.GetNative());
}

void Application::Terminate() {

  std::cout << "Terminating the program..." << '\n';

  if (is_running_) {
    try {
      editorconfig::SaveToDiskManual(); // Force save on close
    } catch (std::exception &ex) {
      std::cerr << "Failed to save editor config: " << ex.what() << std::endl;
    } catch (...) {
      std::cerr << "Failed to save editor config!" << std::endl;
    }

    wgpuInstanceRelease(instance_);
    wgpuSurfaceUnconfigure(surface_);
    wgpuQueueRelease(queue_);
    wgpuSurfaceRelease(surface_);
    wgpuDeviceRelease(device_);

    // glfwDestroyWindow(window_);
    window_.Terminate();
    glfwTerminate();

    ImGui_ImplWGPU_Shutdown();
    ImGui_ImplGlfw_Shutdown();

    std::cout << "Successfully terminated!" << '\n';
  }

  std::cout << "The application is no longer running." << std::endl;

  is_running_ = false;
}

WGPUTextureView Application::GetNextTextureView() {

  WGPUSurfaceTexture surface_texture;
  wgpuSurfaceGetCurrentTexture(surface_, &surface_texture);

  if (surface_texture.status != WGPUSurfaceGetCurrentTextureStatus_SuccessOptimal &&
      surface_texture.status != WGPUSurfaceGetCurrentTextureStatus_SuccessSuboptimal) {
    return nullptr; // TODO: Check for suboptimal
  }

  WGPUTexture texture = surface_texture.texture;

  // Create the texture view
  const WGPUTextureViewDescriptor view_descriptor = {
      .nextInChain = nullptr,
      .label = "Surface Texture View"_w,
      .format = wgpuTextureGetFormat(surface_texture.texture),
      .dimension = WGPUTextureViewDimension_2D,
      .baseMipLevel = 0,
      .mipLevelCount = 1,
      .baseArrayLayer = 0,
      .arrayLayerCount = 1,
      .aspect = WGPUTextureAspect_All,
      .usage = WGPUTextureUsage_RenderAttachment,
  };

  WGPUTextureView target_view = wgpuTextureCreateView(texture, &view_descriptor);

#ifndef WEBGPU_BACKEND_WGPU
  wgpuTextureRelease(surface_texture.texture);
#endif

  return target_view;
}

WGPUSurfaceConfiguration Application::BuildSurfaceConfig(const DeviceWindow &window) const {
  return {
      .nextInChain = nullptr,
      .device = device_,
      .format = preferred_format_,
      .usage = WGPUTextureUsage_RenderAttachment,
      .width = window.GetWidth(),
      .height = window.GetHeight(),
      .viewFormatCount = 0,
      .viewFormats = nullptr,
      .alphaMode = WGPUCompositeAlphaMode_Auto,
#ifndef __EMSCRIPTEN__
      .presentMode = WGPUPresentMode_Immediate,
#else
      .presentMode = WGPUPresentMode_Fifo, // Immediate doesn't work on web...
#endif
  };
}
