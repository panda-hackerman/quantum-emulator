//
// Created by Eli Michaud on 6/4/2026.
//

#ifndef DEVICE_ADAPTER_UTIL_H
#define DEVICE_ADAPTER_UTIL_H
#include <webgpu.h>

// #include <webgpu/webgpu.hpp>

// CALLBACKS
/** Called when the device is lost for some reason */
void OnDeviceLostCallback(WGPUDevice const *device, WGPUDeviceLostReason reason,
                          WGPUStringView message, void *, void *);

/** Called when the device encounters an uncaught error */
void OnDeviceErrorCallback(WGPUDevice const *device, WGPUErrorType type, WGPUStringView message,
                           void *, void *);

// REQUEST SYNC
/**
 * Request an adapter synchronously (while still polling for events, in the meantime).
 * @param instance The WebGPU instance
 * @param options The options to pass to the (async) request adapter function
 * @return The adapter, after it's been returned
 */
WGPUAdapter RequestAdapterSync(WGPUInstance instance, WGPURequestAdapterOptions const *options);

WGPUDevice RequestDeviceSync(WGPUInstance instance, WGPUAdapter adapter,
                             WGPUDeviceDescriptor const *descriptor);

// INSPECT
void InspectAdapter(WGPUAdapter adapter);
void InspectDevice(WGPUDevice device);

#endif // DEVICE_ADAPTER_UTIL_H