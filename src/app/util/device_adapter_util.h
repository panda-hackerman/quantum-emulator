//
// Created by Eli Michaud on 6/4/2026.
//

#ifndef DEVICE_ADAPTER_UTIL_H
#define DEVICE_ADAPTER_UTIL_H

#include <webgpu/webgpu.h>

struct AdapterRequestUserData {
  WGPUAdapter adapter = nullptr;
  bool request_ended = false;
};

struct DeviceRequestUserData {
  WGPUDevice device = nullptr;
  bool request_ended = false;
};

// ADAPTER STUFF
/** Requests an adapter synchronously */
WGPUAdapter RequestAdapterSync(WGPUInstance instance, WGPURequestAdapterOptions const *options);

/** Called after RequestAdapterSync finds the adapter */
void OnAdapterRequestEnded(WGPURequestAdapterStatus status, WGPUAdapter adapter,
                           char const *message, void *user_data_ptr);
// DEVICE STUFF
/** Requests a device synchronously */
WGPUDevice RequestDeviceSync(WGPUAdapter adapter, WGPUDeviceDescriptor const *descriptor);

/** Called after RequestDeviceSync finds the device */
void OnDeviceRequestEnded(WGPURequestDeviceStatus status, WGPUDevice device, char const *message,
                          void *user_data_ptr);

/** Called when the device is lost for some reason */
void OnDeviceLostCallback(WGPUDeviceLostReason reason, char const *message, void *user_data_ptr);

/** Called when the device encounters an uncaught error */
void OnDeviceErrorCallback(WGPUErrorType type, char const *message, void *user_data_ptr);

#endif // DEVICE_ADAPTER_UTIL_H
