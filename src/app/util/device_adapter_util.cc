//
// Created by Eli Michaud on 6/4/2026.
//

#include "device_adapter_util.h"

#include <iostream>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif // __EMSCRIPTEN__

WGPUAdapter RequestAdapterSync(WGPUInstance instance, WGPURequestAdapterOptions const *options) {

  AdapterRequestUserData user_data;
  wgpuInstanceRequestAdapter(instance, options, OnAdapterRequestEnded, &user_data);

#ifdef __EMSCRIPTEN__
  // Hand control back to the browser until the adapter is ready...
  while (!userData.requestEnded) {
    emscripten_sleep(100);
  }
#endif // __EMSCRIPTEN__

  return user_data.adapter;
}

void OnAdapterRequestEnded(WGPURequestAdapterStatus status, WGPUAdapter adapter,
                           char const *message, void *user_data_ptr) {

  AdapterRequestUserData &user_data = *reinterpret_cast<AdapterRequestUserData *>(user_data_ptr);

  if (status == WGPURequestAdapterStatus_Success) {
    user_data.adapter = adapter;
  } else {
    std::cout << "Failed to get WebGPU adapter: " << message << std::endl;
  }

  user_data.request_ended = true;
}

WGPUDevice RequestDeviceSync(WGPUAdapter adapter, WGPUDeviceDescriptor const *descriptor) {

  DeviceRequestUserData user_data;
  wgpuAdapterRequestDevice(adapter, descriptor, OnDeviceRequestEnded, &user_data);

#ifdef __EMSCRIPTEN__
  // Hand control back to the browser until the device is ready
  while (!userData.requestEnded) {
    emscripten_sleep(100);
  }
#endif // __EMSCRIPTEN__

  return user_data.device;
}

void OnDeviceRequestEnded(WGPURequestDeviceStatus status, WGPUDevice device, char const *message,
                          void *user_data_ptr) {
  DeviceRequestUserData &user_data = *reinterpret_cast<DeviceRequestUserData *>(user_data_ptr);

  if (status == WGPURequestDeviceStatus_Success) {
    user_data.device = device;
  } else {
    std::cout << "Failed to get WebGPU Device: " << message << std::endl;
  }

  user_data.request_ended = true;
}

void OnDeviceLostCallback(const WGPUDeviceLostReason reason, char const *message, void * /*data*/) {
  std::cout << "Device lost! (" << reason << ")";
  if (message) std::cout << "\n->" << message;
  std::cout << std::endl;
}

void OnDeviceErrorCallback(const WGPUErrorType type, char const *message, void *user_data_ptr) {
  std::cout << "Device error! (" << type << ")";
  if (message) std::cout << "\n->" << message;
  std::cout << std::endl;
}
