//
// Created by Eli Michaud on 6/10/2026.
//

#ifndef SETTINGS_CONSTANTS_H
#define SETTINGS_CONSTANTS_H
#include <webgpu.h>

constexpr WGPULimits kDeviceRequiredLimits = {
    .nextInChain = nullptr,
    .maxTextureDimension1D = WGPU_LIMIT_U32_UNDEFINED,
    .maxTextureDimension2D = WGPU_LIMIT_U32_UNDEFINED,
    .maxTextureDimension3D = WGPU_LIMIT_U32_UNDEFINED,
    .maxTextureArrayLayers = WGPU_LIMIT_U32_UNDEFINED,
    .maxBindGroups = 2,
    .maxBindGroupsPlusVertexBuffers = WGPU_LIMIT_U32_UNDEFINED,
    .maxBindingsPerBindGroup = WGPU_LIMIT_U32_UNDEFINED,
    .maxDynamicUniformBuffersPerPipelineLayout = WGPU_LIMIT_U32_UNDEFINED,
    .maxDynamicStorageBuffersPerPipelineLayout = WGPU_LIMIT_U32_UNDEFINED,
    .maxSampledTexturesPerShaderStage = WGPU_LIMIT_U32_UNDEFINED,
    .maxSamplersPerShaderStage = WGPU_LIMIT_U32_UNDEFINED,
    .maxStorageBuffersPerShaderStage = WGPU_LIMIT_U32_UNDEFINED,
    .maxStorageTexturesPerShaderStage = WGPU_LIMIT_U32_UNDEFINED,
    .maxUniformBuffersPerShaderStage = WGPU_LIMIT_U32_UNDEFINED,
    .maxUniformBufferBindingSize = WGPU_LIMIT_U64_UNDEFINED, // 64
    .maxStorageBufferBindingSize = WGPU_LIMIT_U64_UNDEFINED, // 64
    .minUniformBufferOffsetAlignment = WGPU_LIMIT_U32_UNDEFINED,
    .minStorageBufferOffsetAlignment = WGPU_LIMIT_U32_UNDEFINED,
    .maxVertexBuffers = WGPU_LIMIT_U32_UNDEFINED,
    .maxBufferSize = WGPU_LIMIT_U64_UNDEFINED, // 64
    .maxVertexAttributes = WGPU_LIMIT_U32_UNDEFINED,
    .maxVertexBufferArrayStride = WGPU_LIMIT_U32_UNDEFINED,
    .maxInterStageShaderVariables = WGPU_LIMIT_U32_UNDEFINED,
    .maxColorAttachments = WGPU_LIMIT_U32_UNDEFINED,
    .maxColorAttachmentBytesPerSample = WGPU_LIMIT_U32_UNDEFINED,
    .maxComputeWorkgroupStorageSize = WGPU_LIMIT_U32_UNDEFINED,
    .maxComputeInvocationsPerWorkgroup = WGPU_LIMIT_U32_UNDEFINED,
    .maxComputeWorkgroupSizeX = WGPU_LIMIT_U32_UNDEFINED,
    .maxComputeWorkgroupSizeY = WGPU_LIMIT_U32_UNDEFINED,
    .maxComputeWorkgroupSizeZ = WGPU_LIMIT_U32_UNDEFINED,
    .maxComputeWorkgroupsPerDimension = WGPU_LIMIT_U32_UNDEFINED,
};

#endif // SETTINGS_CONSTANTS_H
