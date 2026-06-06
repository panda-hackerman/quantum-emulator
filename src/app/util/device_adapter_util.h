//
// Created by Eli Michaud on 6/4/2026.
//

#ifndef DEVICE_ADAPTER_UTIL_H
#define DEVICE_ADAPTER_UTIL_H

#include <webgpu/webgpu.hpp>

// ADAPTER STUFF
/** Called when the device is lost for some reason */
void OnDeviceLostCallback(WGPUDeviceLostReason reason, char const *message, void *user_data_ptr);

/** Called when the device encounters an uncaught error */
void OnDeviceErrorCallback(wgpu::ErrorType type, char const *message);

#endif // DEVICE_ADAPTER_UTIL_H
