//
// Created by Eli Michaud on 6/5/2026.
//

#include "application.h"

#ifdef WEBGPU_BACKEND_WGPU
#include <webgpu/wgpu.h>
#endif

#include <iostream>

#include "glfw3webgpu.h"

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
  const WGPUInstanceDescriptor instance_desc = {
      .nextInChain = nullptr,
  };

#ifdef WEBGPU_BACKEND_EMSCRIPTEN
  WGPUInstance instance = wgpuCreateInstance(nullptr);
#else
  WGPUInstance instance = wgpuCreateInstance(&instance_desc);
#endif

  if (!instance) {
    std::cerr << "Couldn't initialize WebGPU: Failed to create instance :(" << std::endl;
    return false;
  }

  // GET SURFACE
  surface_ = glfwGetWGPUSurface(instance, window_);

  // CREATE ADAPTER
  const WGPURequestAdapterOptions adapter_options = {
      .nextInChain = nullptr,
      .compatibleSurface = surface_,
  };

  WGPUAdapter adapter = RequestAdapterSync(instance, &adapter_options);

  wgpuInstanceRelease(instance);

  // GET DEVICE
  const WGPUDeviceDescriptor device_desc = {
      .nextInChain = nullptr,
      .label = "The Device",
      .requiredFeatureCount = 0,
      .requiredLimits = nullptr,
      .defaultQueue = {.nextInChain = nullptr, .label = "The Default Queue"},
      .deviceLostCallback = OnDeviceLostCallback,
  };

  device_ = RequestDeviceSync(adapter, &device_desc);

  wgpuDeviceSetUncapturedErrorCallback(device_, OnDeviceErrorCallback, nullptr);

  // GET QUEUE
  queue_ = wgpuDeviceGetQueue(device_);

  // CONFIGURE SURFACE
  const WGPUSurfaceConfiguration surface_config = {
      .nextInChain = nullptr,
      .device = device_,
      .format = wgpuSurfaceGetPreferredFormat(surface_, adapter),
      .usage = WGPUTextureUsage_RenderAttachment,
      .viewFormatCount = 0,
      .viewFormats = nullptr,
      .alphaMode = WGPUCompositeAlphaMode_Auto,
      .width = kWindowWidth,
      .height = kWindowHeight,
      .presentMode = WGPUPresentMode_Fifo,
  };

  wgpuSurfaceConfigure(surface_, &surface_config);

  // Release stuff we don't need anymore
  wgpuAdapterRelease(adapter);

  return true;
}

void Application::Tick() {
  glfwPollEvents();

  // Get the next target view
  auto [surface_texture, target_view] = GetNextTextureView();
  if (!target_view) return;

  // GET ENCODER
  const WGPUCommandEncoderDescriptor encoder_desc = {
      .nextInChain = nullptr,
      .label = "The Command Encoder",
  };

  WGPUCommandEncoder encoder = wgpuDeviceCreateCommandEncoder(device_, &encoder_desc);

  // GET RENDER PASS
  const WGPURenderPassColorAttachment color_attachment = {
      .view = target_view,
      .resolveTarget = nullptr,
      .loadOp = WGPULoadOp_Clear,
      .storeOp = WGPUStoreOp_Store,
      .clearValue = WGPUColor{100.0 / 255, 149.0 / 255, 237.0 / 255, 1.0},
#ifndef WEBGPU_BACKEND_WGPU
      .depthSlice = WGPU_DEPTH_SLICE_UNDEFINED,
#endif
  };

  const WGPURenderPassDescriptor render_pass_desc = {
      .nextInChain = nullptr,
      .colorAttachmentCount = 1,
      .colorAttachments = &color_attachment,
      .depthStencilAttachment = nullptr,
      .timestampWrites = nullptr,
  };

  // CREATE RENDER PASS
  // rn we only clear the screen so we just kill it immediately
  WGPURenderPassEncoder render_pass = wgpuCommandEncoderBeginRenderPass(encoder, &render_pass_desc);
  wgpuRenderPassEncoderEnd(render_pass);
  wgpuRenderPassEncoderRelease(render_pass);

  // ENCODE AND SUBMIT RENDER PASS
  WGPUCommandBufferDescriptor cmd_buffer_desc = {
      .nextInChain = nullptr,
      .label = "The Command Buffer",
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

// #ifdef WEBGPU_BACKEND_WGPU
//   wgpuTextureRelease(surface_texture.texture);
// #endif

#if defined(WEBGPU_BACKEND_DAWN)
  wgpuDeviceTick(device);
#elif defined(WEBGPU_BACKEND_WGPU)
  wgpuDevicePoll(device_, false, nullptr);
#elif defined(WEBGPU_BACKEND_EMSCRIPTEN)
  emscripten_sleep(100); //FIXME: Emscripten
#endif

}

void Application::Terminate() {
  wgpuSurfaceUnconfigure(surface_);
  wgpuQueueRelease(queue_);
  wgpuSurfaceRelease(surface_);
  wgpuDeviceRelease(device_);

  glfwDestroyWindow(window_);
  glfwTerminate();
}

bool Application::ShouldContinue() const {
  return !glfwWindowShouldClose(window_);
}

Application::TextureViewPair Application::GetNextTextureView() {
  WGPUSurfaceTexture surface_texture;
  wgpuSurfaceGetCurrentTexture(surface_, &surface_texture);

  if (surface_texture.status != WGPUSurfaceGetCurrentTextureStatus_Success) {
    return {surface_texture, nullptr};
  }

  // TODO: Check for suboptimal

  // Create the texture view
  const WGPUTextureViewDescriptor view_descriptor = {
      .nextInChain = nullptr,
      .label = "Surface Texture View",
      .format = wgpuTextureGetFormat(surface_texture.texture),
      .dimension = WGPUTextureViewDimension_2D,
      .baseMipLevel = 0,
      .mipLevelCount = 1,
      .baseArrayLayer = 0,
      .arrayLayerCount = 1,
      .aspect = WGPUTextureAspect_All,
  };

  WGPUTextureView view = wgpuTextureCreateView(surface_texture.texture, &view_descriptor);

#ifndef WEBGPU_BACKEND_WGPU
  wgpuTextureRelease(surface_texture.texture);
#endif

  return {surface_texture, view};
}
