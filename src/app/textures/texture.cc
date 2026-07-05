//
// Created by Eli Michaud on 7/4/2026.
//

#define STB_IMAGE_IMPLEMENTATION
#include "texture.h"

#include "../util/wgpu_string_view_util.h"
#include "stb_image.h"

void TextureManager::LoadAllTextures(WGPUDevice device) {
  if (textures_loaded_) return;

  int i = 0;
  for (auto [texture_id, path] : kTextureToPathMap) {
    textures_[i] = LoadTextureFromFile(path, device);
    id_to_texture_.emplace(texture_id, &textures_[i]);

    i++;
  }

  textures_loaded_ = true;
}

void TextureManager::UnloadAllTextures() {
  if (!textures_loaded_) return;

  for (int i = 0; i < textures_.size(); ++i) {
    textures_[i].Unload();
    textures_[i] = {};
  }

  id_to_texture_ = {};

  textures_loaded_ = false;
}

const Texture *TextureManager::GetTexture(const TextureID id) const {
  if (!textures_loaded_) {
    throw std::runtime_error("Tried to get texture, but didn't call LoadAllTextures() first!");
  }

  return id_to_texture_.at(id);
}

Texture TextureManager::LoadTextureFromFile(const fs::path &path, WGPUDevice device,
                            const WGPUTextureFormat format) {

  int width, height, channels;
  const std::string path_str = path.string();

  unsigned char *pixel_data = stbi_load(path_str.data(), &width, &height, &channels, 4);

  if (pixel_data == nullptr) return {};

  const WGPUExtent3D texture_size = {static_cast<uint32_t>(width), static_cast<uint32_t>(height),
                                     1};
  const WGPUTextureDescriptor texture_desc = {
      .nextInChain = nullptr,
      .usage = WGPUTextureUsage_TextureBinding | WGPUTextureUsage_CopyDst,
      .dimension = WGPUTextureDimension_2D,
      .size = texture_size,
      .format = format,
      .mipLevelCount = 1,
      .sampleCount = 1,
      .viewFormatCount = 0,
      .viewFormats = nullptr,
  };

  WGPUTexture texture = wgpuDeviceCreateTexture(device, &texture_desc);

  /* WRITE TEXTURE */ {
    const WGPUTexelCopyTextureInfo destination = {
        .texture = texture,
        .mipLevel = 0,
        .origin = {0, 0, 0},
        .aspect = WGPUTextureAspect_All,
    };

    const WGPUTexelCopyBufferLayout source = {
        .offset = 0,
        .bytesPerRow = texture_size.width * 4,
        .rowsPerImage = texture_size.height,
    };

    const std::size_t data_size = width * height * 4;
    WGPUQueue queue = wgpuDeviceGetQueue(device);

    wgpuQueueWriteTexture(queue, &destination, pixel_data, data_size, &source, &texture_size);
    wgpuQueueRelease(queue);
  }

  stbi_image_free(pixel_data);

  const WGPUTextureViewDescriptor texture_view_desc = {
      .nextInChain = nullptr,
      .label = "Generated Texture View"_w,
      .format = format,
      .dimension = WGPUTextureViewDimension_2D,
      .baseMipLevel = 0,
      .mipLevelCount = texture_desc.mipLevelCount,
      .baseArrayLayer = 0,
      .arrayLayerCount = 1,
      .aspect = WGPUTextureAspect_All,
      .usage = texture_desc.usage,
  };

  WGPUTextureView view = wgpuTextureCreateView(texture, &texture_view_desc);

  return {texture, view, width, height};
}