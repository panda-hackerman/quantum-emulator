//
// Created by Eli Michaud on 6/4/2026.
//

#include "device_adapter_util.h"

#include <iostream>

#include "sleep_util.h"
#include "wgpu_string_view_util.h"

void OnDeviceLostCallback(WGPUDevice const *, const WGPUDeviceLostReason reason,
                          WGPUStringView message, void *, void *) {
  std::cout << "Device lost! (" << reason << ")";

  if (message.data != nullptr) {
    std::cout << "\n->" << message;
  }

  std::cout << std::endl;
}

void OnDeviceErrorCallback(WGPUDevice const *, WGPUErrorType type, WGPUStringView message, void *,
                           void *) {
  std::cout << "Device error! (" << type << ")";

  if (message.data != nullptr) {
    std::cout << "\n->" << message;
  }

  std::cout << std::endl;
}

WGPUAdapter RequestAdapterSync(WGPUInstance instance, WGPURequestAdapterOptions const *options) {

  struct UserData {
    WGPUAdapter adapter = nullptr;
    bool request_ended = false;
  };

  UserData user_data;

  auto on_req_ended = [](const WGPURequestAdapterStatus status, WGPUAdapter adapter,
                         WGPUStringView message, void *userdata_1, void * /*userdata_2*/) {
    UserData &user_data = *reinterpret_cast<UserData *>(userdata_1);

    if (status == WGPURequestAdapterStatus_Success) {
      user_data.adapter = adapter;
    } else {
      std::cerr << "Error getting the adapter: " << message << std::endl;
    }

    user_data.request_ended = true;
  };

  WGPURequestAdapterCallbackInfo callback_info = {
      .nextInChain = nullptr,
      .mode = WGPUCallbackMode_AllowProcessEvents,
      .callback = on_req_ended,
      .userdata1 = &user_data,
      .userdata2 = nullptr,
  };

  wgpuInstanceRequestAdapter(instance, options, callback_info);

  wgpuInstanceProcessEvents(instance);

  while (!user_data.request_ended) {
    SleepForMs(200);
    wgpuInstanceProcessEvents(instance);
  }

  return user_data.adapter;
}

WGPUDevice RequestDeviceSync(WGPUInstance instance, WGPUAdapter adapter,
                             WGPUDeviceDescriptor const *descriptor) {
  struct UserData {
    WGPUDevice device = nullptr;
    bool request_ended = false;
  };

  UserData user_data;

  auto on_device_req_ended = [](WGPURequestDeviceStatus status, WGPUDevice device,
                                WGPUStringView message, void *userdata_1, void * /* userdata_2 */) {
    UserData &user_data = *reinterpret_cast<UserData *>(userdata_1);

    if (status == WGPURequestDeviceStatus_Success) {
      user_data.device = device;
    } else {
      std::cerr << "Error getting the device:" << message << std::endl;
    }

    user_data.request_ended = true;
  };

  WGPURequestDeviceCallbackInfo callback_info = {
      .nextInChain = nullptr,
      .mode = WGPUCallbackMode_AllowProcessEvents,
      .callback = on_device_req_ended,
      .userdata1 = &user_data,
      .userdata2 = nullptr,
  };

  wgpuAdapterRequestDevice(adapter, descriptor, callback_info);

  wgpuInstanceProcessEvents(instance);

  while (!user_data.request_ended) {
    SleepForMs(200);
    wgpuInstanceProcessEvents(instance);
  }

  return user_data.device;
}

void InspectAdapter(WGPUAdapter adapter) {
  // Log Supported Limits
  WGPULimits limits = {
      .nextInChain = nullptr,
  };

  if (wgpuAdapterGetLimits(adapter, &limits) == WGPUStatus_Success) {
    std::cout << '\n' << "ADAPTER LIMITS: " << '\n';
    std::cout << " -> maxTextureDimension1D: " << limits.maxTextureDimension1D << '\n';
    std::cout << " -> maxTextureDimension2D: " << limits.maxTextureDimension2D << '\n';
    std::cout << " -> maxTextureDimension3D: " << limits.maxTextureDimension3D << '\n';
    std::cout << " -> maxTextureArrayLayers: " << limits.maxTextureArrayLayers << '\n';
  }

  // Log Supported Features
  WGPUSupportedFeatures features;

  wgpuAdapterGetFeatures(adapter, &features);

  std::cout << '\n' << "ADAPTER FEATURES: " << '\n';

  for (size_t i = 0; i < features.featureCount; ++i) {
    std::cout << " -> 0x" << std::hex << features.features[i] << std::dec << "\n";
  }

  wgpuSupportedFeaturesFreeMembers(features);

  // Log Additional Properties
  WGPUAdapterInfo properties = {.nextInChain = nullptr};

  if (wgpuAdapterGetInfo(adapter, &properties) == WGPUStatus_Success) {
    std::cout << '\n' << "ADAPTER PROPERTIES: " << '\n';

    std::cout << " -> vendorID: " << properties.vendorID << '\n';
    std::cout << " -> vendorName: " << properties.vendor << '\n';
    std::cout << " -> architecture: " << properties.architecture << '\n';
    std::cout << " -> deviceID: " << properties.deviceID << '\n';
    std::cout << " -> name: " << properties.device << '\n';
    std::cout << " -> driverDescription: " << properties.description << '\n';
    std::cout << " -> adapterType: 0x" << std::hex << properties.adapterType << std::dec << '\n';
    std::cout << " -> backendType: 0x" << std::hex << properties.backendType << std::dec << '\n';

    wgpuAdapterInfoFreeMembers(properties);
  }

  std::cout << std::endl;
}

void InspectDevice(WGPUDevice device) {

  WGPUSupportedFeatures features;

  // Log Supported Features
  wgpuDeviceGetFeatures(device, &features);

  std::cout << '\n' << "DEVICE FEATURES: " << '\n';

  for (std::size_t i = 0; i < features.featureCount; ++i) {
    std::cout << " -> 0x" << std::hex << features.features[i] << std::dec << '\n';
  }

  wgpuSupportedFeaturesFreeMembers(features);

  // Log Limits
  WGPULimits limits = {};

  if (wgpuDeviceGetLimits(device, &limits) == WGPUStatus_Success) {
    std::cout << '\n' << "DEVICE LIMITS:" << '\n';
    std::cout << " -> maxTextureDimension1D: " << limits.maxTextureDimension1D << '\n';
    std::cout << " -> maxTextureDimension2D: " << limits.maxTextureDimension2D << '\n';
    std::cout << " -> maxTextureDimension3D: " << limits.maxTextureDimension3D << '\n';
    std::cout << " -> maxTextureArrayLayers: " << limits.maxTextureArrayLayers << '\n';
    std::cout << " -> maxBindGroups: " << limits.maxBindGroups << '\n';
    std::cout << " -> maxBindGroupsPlusVertexBuffers: " << limits.maxBindGroupsPlusVertexBuffers << '\n';
    std::cout << " -> maxBindingsPerBindGroup: " << limits.maxBindingsPerBindGroup << '\n';
    std::cout << " -> maxDynamicUniformBuffersPerPipelineLayout: " << limits.maxDynamicUniformBuffersPerPipelineLayout << '\n';
    std::cout << " -> maxDynamicStorageBuffersPerPipelineLayout: " << limits.maxDynamicStorageBuffersPerPipelineLayout << '\n';
    std::cout << " -> maxSampledTexturesPerShaderStage: " << limits.maxSampledTexturesPerShaderStage << '\n';
    std::cout << " -> maxSamplersPerShaderStage: " << limits.maxSamplersPerShaderStage << '\n';
    std::cout << " -> maxStorageBuffersPerShaderStage: " << limits.maxStorageBuffersPerShaderStage << '\n';
    std::cout << " -> maxStorageTexturesPerShaderStage: " << limits.maxStorageTexturesPerShaderStage << '\n';
    std::cout << " -> maxUniformBuffersPerShaderStage: " << limits.maxUniformBuffersPerShaderStage << '\n';
    std::cout << " -> maxUniformBufferBindingSize: " << limits.maxUniformBufferBindingSize << '\n';
    std::cout << " -> maxStorageBufferBindingSize: " << limits.maxStorageBufferBindingSize << '\n';
    std::cout << " -> minUniformBufferOffsetAlignment: " << limits.minUniformBufferOffsetAlignment << '\n';
    std::cout << " -> minStorageBufferOffsetAlignment: " << limits.minStorageBufferOffsetAlignment << '\n';
    std::cout << " -> maxVertexBuffers: " << limits.maxVertexBuffers << '\n';
    std::cout << " -> maxBufferSize: " << limits.maxBufferSize << '\n';
    std::cout << " -> maxVertexAttributes: " << limits.maxVertexAttributes << '\n';
    std::cout << " -> maxVertexBufferArrayStride: " << limits.maxVertexBufferArrayStride << '\n';
    std::cout << " -> maxInterStageShaderVariables: " << limits.maxInterStageShaderVariables << '\n';
    std::cout << " -> maxColorAttachments: " << limits.maxColorAttachments << '\n';
    std::cout << " -> maxColorAttachmentBytesPerSample: " << limits.maxColorAttachmentBytesPerSample << '\n';
    std::cout << " -> maxComputeWorkgroupStorageSize: " << limits.maxComputeWorkgroupStorageSize << '\n';
    std::cout << " -> maxComputeInvocationsPerWorkgroup: " << limits.maxComputeInvocationsPerWorkgroup << '\n';
    std::cout << " -> maxComputeWorkgroupSizeX: " << limits.maxComputeWorkgroupSizeX << '\n';
    std::cout << " -> maxComputeWorkgroupSizeY: " << limits.maxComputeWorkgroupSizeY << '\n';
    std::cout << " -> maxComputeWorkgroupSizeZ: " << limits.maxComputeWorkgroupSizeZ << '\n';
    std::cout << " -> maxComputeWorkgroupsPerDimension: " << limits.maxComputeWorkgroupsPerDimension << '\n';
  }

  std::cout << std::endl;
}
