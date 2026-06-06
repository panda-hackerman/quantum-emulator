//
// Created by Eli Michaud on 6/5/2026.
//

#include "application.h"

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
  wgpu::Instance instance = wgpuCreateInstance(nullptr);

  // GET SURFACE
  surface_ = glfwGetWGPUSurface(instance, window_);

  // CREATE ADAPTER
  const wgpu::RequestAdapterOptions adapter_options = {{
      .compatibleSurface = surface_,
  }};

  wgpu::Adapter adapter = instance.requestAdapter(adapter_options);

  instance.release();

  // GET DEVICE
  const wgpu::DeviceDescriptor device_desc = {{
      .label = "The Device",
      .requiredFeatureCount = 0,
      .requiredLimits = nullptr,
      .defaultQueue = {.nextInChain = nullptr, .label = "The Default Queue"},
      .deviceLostCallback = OnDeviceLostCallback,
  }};

  device_ = adapter.requestDevice(device_desc);
  device_.setUncapturedErrorCallback(OnDeviceErrorCallback);

  // GET QUEUE
  queue_ = device_.getQueue();
  // queue_ = wgpuDeviceGetQueue(device_);

  // CONFIGURE SURFACE
  const wgpu::SurfaceConfiguration surface_config = {{
      .device = device_,
      .format = wgpuSurfaceGetPreferredFormat(surface_, adapter),
      .usage = WGPUTextureUsage_RenderAttachment,
      .viewFormatCount = 0,
      .viewFormats = nullptr,
      .alphaMode = WGPUCompositeAlphaMode_Auto,
      .width = kWindowWidth,
      .height = kWindowHeight,
      .presentMode = WGPUPresentMode_Fifo,
  }};

  surface_.configure(surface_config);

  // Release stuff we don't need anymore
  adapter.release();
  // wgpuAdapterRelease(adapter);

  return true;
}

void Application::Tick() {
  glfwPollEvents();

  // Get the next target view
  auto target_view = GetNextTextureView();
  if (!target_view) return;

  // GET ENCODER
  const wgpu::CommandEncoderDescriptor encoder_desc = {{
      .label = "The Command Encoder",
  }};

  wgpu::CommandEncoder encoder = device_.createCommandEncoder(encoder_desc);

  // GET RENDER PASS
  const wgpu::RenderPassColorAttachment color_attachment = {{
      .view = target_view,
      .resolveTarget = nullptr,
      .loadOp = WGPULoadOp_Clear,
      .storeOp = WGPUStoreOp_Store,
      .clearValue = WGPUColor{100.0 / 255, 149.0 / 255, 237.0 / 255, 1.0},
#ifndef WEBGPU_BACKEND_WGPU
      .depthSlice = WGPU_DEPTH_SLICE_UNDEFINED,
#endif
  }};

  const wgpu::RenderPassDescriptor render_pass_desc = {{
      .colorAttachmentCount = 1,
      .colorAttachments = &color_attachment,
      .depthStencilAttachment = nullptr,
      .timestampWrites = nullptr,
  }};

  // CREATE RENDER PASS
  // rn we only clear the screen so we just kill it immediately
  wgpu::RenderPassEncoder render_pass = encoder.beginRenderPass(render_pass_desc);
  render_pass.end();
  render_pass.release();

  // ENCODE AND SUBMIT RENDER PASS
  const wgpu::CommandBufferDescriptor cmd_buffer_desc = {{
      .label = "The Command Buffer",
  }};

  wgpu::CommandBuffer command = encoder.finish(cmd_buffer_desc);
  encoder.release();

  queue_.submit(1, &command);
  command.release();

  // RELEASING STUFF
  target_view.release();

  // PRESENTING STUFF
#ifndef __EMSCRIPTEN__
  surface_.present();
  // wgpuSurfacePresent(surface_);
#endif

// #ifdef WEBGPU_BACKEND_WGPU
//   wgpuTextureRelease(surface_texture.texture);
// #endif

#if defined(WEBGPU_BACKEND_DAWN)
  device_.tick();
#elif defined(WEBGPU_BACKEND_WGPU)
  device_.poll(false);
#elif defined(WEBGPU_BACKEND_EMSCRIPTEN)
  emscripten_sleep(100); // FIXME: Emscripten
#endif
}

void Application::Terminate() {

  if (has_terminated_) return;

  surface_.unconfigure();
  queue_.release();
  surface_.release();
  device_.release();

  glfwDestroyWindow(window_);
  glfwTerminate();

  has_terminated_ = true;
}

bool Application::ShouldContinue() const {
  return !glfwWindowShouldClose(window_);
}

wgpu::TextureView Application::GetNextTextureView() {

  wgpu::SurfaceTexture surface_texture;
  surface_.getCurrentTexture(&surface_texture);
  // wgpuSurfaceGetCurrentTexture(surface_, &surface_texture);

  if (surface_texture.status != wgpu::SurfaceGetCurrentTextureStatus::Success) {
    return nullptr;
  }

  wgpu::Texture texture = surface_texture.texture;

  // TODO: Check for suboptimal

  // Create the texture view
  const wgpu::TextureViewDescriptor view_descriptor = {{
      .nextInChain = nullptr,
      .label = "Surface Texture View",
      .format = wgpuTextureGetFormat(surface_texture.texture),
      .dimension = WGPUTextureViewDimension_2D,
      .baseMipLevel = 0,
      .mipLevelCount = 1,
      .baseArrayLayer = 0,
      .arrayLayerCount = 1,
      .aspect = WGPUTextureAspect_All,
  }};

  wgpu::TextureView target_view = texture.createView(view_descriptor);

#ifndef WEBGPU_BACKEND_WGPU
  wgpuTextureRelease(surface_texture.texture);
#endif

  return target_view;
}
