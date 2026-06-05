//
// Created by Eli Michaud on 6/5/2026.
//

#include "application.h"

#include <iostream>

#include "glfw3webgpu.h"

bool Application::Init() {

  // CREATE INSTANCE
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

  // GET DEVICE
  const WGPUDeviceDescriptor device_desc = {
    .nextInChain = nullptr,
    .label = "The Device",
    .requiredFeatureCount = 0,
    .requiredLimits = nullptr,
    .defaultQueue = {
      .nextInChain = nullptr,
      .label = "The Default Queue"
    },
    .deviceLostCallback = OnDeviceLostCallback,
  };

  device_ = RequestDeviceSync(adapter, &device_desc);

  wgpuDeviceSetUncapturedErrorCallback(device_, OnDeviceErrorCallback, nullptr);

  // GET QUEUE
  queue_ = wgpuDeviceGetQueue(device_);

  // Release stuff we don't need anymore
  wgpuAdapterRelease(adapter);
  wgpuInstanceRelease(instance);

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

  return true;
}

void Application::Terminate() {
  wgpuQueueRelease(queue_);
  wgpuSurfaceRelease(surface_);
  wgpuDeviceRelease(device_);

  glfwDestroyWindow(window_);
  glfwTerminate();
}

void Application::Tick() {
  glfwPollEvents();
}

bool Application::ShouldContinue() const {
  return !glfwWindowShouldClose(window_);
}
