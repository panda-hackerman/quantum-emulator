//
// Created by Eli Michaud on 6/5/2026.
//

#include "application.h"

#include <glfw3webgpu.h>
#include <imgui.h>

#include <iostream>

#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_wgpu.h"
#include "util/device_adapter_util.h"
#include "util/settings_constants.h"
#include "util/wgpu_string_view_util.h"

bool Application::Init() {

  // GLFW SETUP
  if (!glfwInit()) {
    std::cerr << "Couldn't initialize GLFW: Init function failed..." << std::endl;
    return false;
  }

  // Create window
  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
  window_ = glfwCreateWindow(kWindowWidth, kWindowHeight, "Quantum Emulator", nullptr, nullptr);

  if (!window_) {
    std::cerr << "Couldn't create GLFW window... " << std::endl;
    return false;
  }

  // CREATE WGPU INSTANCE
#ifdef WEBGPU_BACKEND_EMSCRIPTEN
  instance_ = wgpuCreateInstance(nullptr);
#else
  const WGPUInstanceDescriptor instance_desc = {.nextInChain = nullptr};
  instance_ = wgpuCreateInstance(&instance_desc);
#endif

  // GET SURFACE
  surface_ = glfwCreateWindowWGPUSurface(instance_, window_); // FIXME

  // REQUEST ADAPTER
  std::cout << "Requesting Adapter..." << '\n';

  const WGPURequestAdapterOptions adapter_options = {
      .nextInChain = nullptr,
      .compatibleSurface = surface_,
  };

  WGPUAdapter adapter = RequestAdapterSync(instance_, &adapter_options);

  std::cout << "Got Adapter: " << adapter << '\n';

  InspectAdapter(adapter); // DEBUG

  // wgpuInstanceRelease(instance_);

  // GET DEVICE
  std::cout << "Requesting Device..." << '\n';

  WGPUDeviceDescriptor device_desc = {
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

  InspectDevice(device_);

  // GET QUEUE
  std::cout << "\nGetting Queue: " << '\n';
  queue_ = wgpuDeviceGetQueue(device_);
  std::cout << "Got Queue: " << queue_ << '\n';

  // CONFIGURE SURFACE
  std::cout << "\nConfiguring Surface: " << '\n';
  WGPUSurfaceCapabilities capabilities = {};
  wgpuSurfaceGetCapabilities(surface_, adapter, &capabilities);

  std::cout << "Got surface capabilities..." << '\n';

  const WGPUTextureFormat preferred_format = capabilities.formats[0];

  const WGPUSurfaceConfiguration surface_config = {
      .nextInChain = nullptr,
      .device = device_,
      .format = preferred_format,
      .usage = WGPUTextureUsage_RenderAttachment,
      .width = kWindowWidth,
      .height = kWindowHeight,
      .viewFormatCount = 0,
      .viewFormats = nullptr,
      .alphaMode = WGPUCompositeAlphaMode_Auto,
#ifndef __EMSCRIPTEN__
      .presentMode = WGPUPresentMode_Immediate,
#else
      .presentMode = WGPUPresentMode_Fifo, // Immediate doesn't work on web...
#endif
  };

  wgpuSurfaceConfigure(surface_, &surface_config);
  wgpuSurfaceCapabilitiesFreeMembers(capabilities);

  std::cout << "Configured surface successfully! " << '\n';

  // Release stuff we don't need anymore
  wgpuAdapterRelease(adapter);

  // INIT GUI
  std::cout << "\nInitializing ImGui..." << '\n';

  ImGui::CreateContext();
  ImGui::GetIO();

  ImGui_ImplGlfw_InitForOther(window_, true);

  ImGui_ImplWGPU_InitInfo init_info;
  init_info.Device = device_;
  init_info.RenderTargetFormat = preferred_format;

  ImGui_ImplWGPU_Init(&init_info);

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
  static float f = 0.0f;
  static int counter = 0;
  static bool show_demo_window = true;
  static bool show_another_window = false;
  static auto clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

  ImGui::Begin("Hello, world!");

  ImGui::Text("This is some useful text.");
  ImGui::Checkbox("Demo Window", &show_demo_window);
  ImGui::Checkbox("Another Window", &show_another_window);

  ImGui::SliderFloat("float", &f, 0.0f, 1.0f);
  ImGui::ColorEdit3("clear color", reinterpret_cast<float *>(&clear_color));

  if (ImGui::Button("Button")) counter++;
  ImGui::SameLine();
  ImGui::Text("counter = %d", counter);

  ImGuiIO &io = ImGui::GetIO();
  ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
  ImGui::End();
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
      .clearValue = kWindowClearColor,
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
  // DrawFrame(render_pass); // FIXME: Render

  wgpuRenderPassEncoderEnd(render_pass);
  wgpuRenderPassEncoderRelease(render_pass);

  // ENCODE AND SUBMIT RENDER PASS
  WGPUCommandBufferDescriptor cmd_buffer_desc = {
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

void Application::Terminate() {

  std::cout << "Terminating the program..." << '\n';

  if (is_running_) {
    wgpuInstanceRelease(instance_);
    wgpuSurfaceUnconfigure(surface_);
    wgpuQueueRelease(queue_);
    wgpuSurfaceRelease(surface_);
    wgpuDeviceRelease(device_);

    glfwDestroyWindow(window_);
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