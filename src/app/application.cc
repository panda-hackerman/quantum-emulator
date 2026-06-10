//
// Created by Eli Michaud on 6/5/2026.
//

#include "application.h"

#include <iostream>

#include "glfw3webgpu.h"
// #include "imgui.h"
#include <wgpu.h>

#include "util/device_adapter_util.h"
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

  wgpuInstanceRelease(instance_);

  // GET DEVICE
  std::cout << "Requesting Device..." << '\n';

  WGPUDeviceDescriptor device_desc = {
      .nextInChain = nullptr,
      .label = "The Device"_w,
      .requiredFeatureCount = 0,
      .requiredFeatures = nullptr,
      .requiredLimits = nullptr,
      .defaultQueue = {.nextInChain = nullptr, .label = "The Default Queue"_w},
      .deviceLostCallbackInfo = {.nextInChain = nullptr,
                                 .mode = WGPUCallbackMode_AllowProcessEvents,
                                 .callback = OnDeviceLostCallback},
      .uncapturedErrorCallbackInfo = {.nextInChain = nullptr, .callback = OnDeviceErrorCallback},
  };

  device_ = RequestDeviceSync(instance_, adapter, &device_desc);
  // wgpuAdapterRelease(adapter);

  std::cout << "Got Device: " << device_ << '\n';

  InspectDevice(device_);

  // GET QUEUE
  queue_ = wgpuDeviceGetQueue(device_);
  // queue_ = device_.getQueue();

  // CONFIGURE SURFACE
  WGPUSurfaceCapabilities capabilities = {};
  wgpuSurfaceGetCapabilities(surface_, adapter, &capabilities);

  const WGPUTextureFormat &preferred_format = capabilities.formats[0];

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
      .presentMode = WGPUPresentMode_Fifo,
  };

  wgpuSurfaceConfigure(surface_, &surface_config);
  wgpuSurfaceCapabilitiesFreeMembers(capabilities);

  // Release stuff we don't need anymore
  wgpuAdapterRelease(adapter);

  // INIT GUI
  InitGUI();
  // if (!InitGUI()) {
  //   std::cerr << "Couldn't initialize the GUI!" << std::endl;
  //   return false;
  // }

  is_running_ = true;
  return true;
}

bool Application::InitGUI() {

  // IMGUI_CHECKVERSION();

  std::cout << "Init GUI..." << std::endl;

  // ImGui::CreateContext();
  // ImGui::GetIO();
  //
  // ImGui_ImplGlfw_InitForOther(window_, true);
  //
  // ImGui_ImplWGPU_Init(device_, 3, WGPUTextureFormat_Undefined, WGPUTextureFormat_Undefined);

  return true;
}

void Application::TerminateGUI() {
  // ImGui_ImplGlfw_Shutdown();
  // ImGui_ImplWGPU_Shutdown();
}

void Application::Tick() {
  glfwPollEvents();
  wgpuInstanceProcessEvents(instance_);

  // Get the next target view
  WGPUTextureView target_view = GetNextTextureView();
  if (!target_view) return;

  // GET ENCODER
  const WGPUCommandEncoderDescriptor encoder_desc = {
      .nextInChain = nullptr,
      .label = "The Command Encoder"_w,
  };

  WGPUCommandEncoder encoder = wgpuDeviceCreateCommandEncoder(device_, &encoder_desc);
  // WGPUCommandEncoder encoder = device_.createCommandEncoder(encoder_desc);

  // GET RENDER PASS
  const WGPURenderPassColorAttachment color_attachment = {
      .view = target_view,
#ifndef WEBGPU_BACKEND_WGPU
      .depthSlice = WGPU_DEPTH_SLICE_UNDEFINED,
#endif
      .resolveTarget = nullptr,
      .loadOp = WGPULoadOp_Clear,
      .storeOp = WGPUStoreOp_Store,
      .clearValue = WGPUColor{100.0 / 255, 149.0 / 255, 237.0 / 255, 1.0},
  };

  const WGPURenderPassDescriptor render_pass_desc = {
      .colorAttachmentCount = 1,
      .colorAttachments = &color_attachment,
      .depthStencilAttachment = nullptr,
      .timestampWrites = nullptr,
  };

  // CREATE RENDER PASS
  // rn we only clear the screen so we just kill it immediately
  WGPURenderPassEncoder render_pass = wgpuCommandEncoderBeginRenderPass(encoder, &render_pass_desc);

  // TODO: Render stuff...

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

#if defined(WEBGPU_BACKEND_DAWN)
  wgpuDeviceTick()
  // device_.tick();
#elif defined(WEBGPU_BACKEND_WGPU)
  wgpuDevicePoll(device_, false, nullptr);
#elif defined(WEBGPU_BACKEND_EMSCRIPTEN)
  emscripten_sleep(100); // FIXME: Emscripten
#endif
}

void Application::Terminate() {

  if (!is_running_) return;

  wgpuSurfaceUnconfigure(surface_);
  wgpuQueueRelease(queue_);
  wgpuSurfaceRelease(surface_);
  wgpuDeviceRelease(device_);

  glfwDestroyWindow(window_);
  glfwTerminate();

  TerminateGUI();

  is_running_ = false;
}

bool Application::ShouldContinue() const {
  return !glfwWindowShouldClose(window_);
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