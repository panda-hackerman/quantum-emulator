//
// Created by Eli Michaud on 7/4/2026.
//

#ifndef TEXTURE_H
#define TEXTURE_H
#include <filesystem>
#include <map>
#include <webgpu/webgpu.hpp>

#include "collections/compile_time_map.h"
#include "imgui.h"

namespace fs = std::filesystem;

enum class TextureID : uint8_t {
  kUndefined = 0,
  kCircuit,
};

enum class SpriteID : uint8_t {
  kUndefined = 0,

  /* TextureID::kCircuit Sprites: */

  kCircuitIdentity,
  kCircuitHadamard,
  kCircuitTGate,
  kCircuitPhase,
  kCircuitPauliX,
  kCircuitPauliY,
  kCircuitPauliZ,
  kCircuitGeneric,
  kCircuitControl,
  kCircuitAntiControl,
  kCircuitSwap,
  kCircuitMeasure,
};

/**
 * Wrapper over WGPUTexture and WGPUTextureView.
 * @warning If not handling via TextureManager, make sure to call Unload!!
 */
struct Texture {
  WGPUTexture texture = nullptr;
  WGPUTextureView view = nullptr;
  int width = 0;
  int height = 0;

  [[nodiscard]] ImTextureRef GetViewRef() const {
    return static_cast<ImTextureID>(reinterpret_cast<intptr_t>(view));
  }

  [[nodiscard]] ImVec2 Size() const {
    return {static_cast<float>(width), static_cast<float>(height)};
  }

  [[nodiscard]] bool Empty() const { return texture == nullptr || view == nullptr; }

  void Unload() const {
    if (texture || view) {
      wgpuTextureViewRelease(view);
      wgpuTextureRelease(texture);
    }
  }

  Texture() = default;

  Texture(WGPUTexture texture, WGPUTextureView view, const int width, const int height) :
      texture{texture}, view{view}, width{width}, height{height} {}
};

/**
 * Represents a section of a texture.
 */
struct Sprite {
  TextureID texture_id;
  float x, y, width, height;

  [[nodiscard]] ImVec2 GetUV1(const ImVec2 size) const noexcept {
    return ImVec2{x / size.x, y / size.y};
  }

  [[nodiscard]] ImVec2 GetUV2(const ImVec2 size) const noexcept {
    return ImVec2{(x + width) / size.x, (y + height) / size.y};
  }
};

constexpr auto kTextureToPathMap = CTMapBuilder<TextureID, const char *>({
    {TextureID::kCircuit, RESOURCE_DIR "/textures/circuit_texturemap.png"},
});

constexpr auto kIdToSpriteMap = CTMapBuilder<SpriteID, Sprite>({
    {SpriteID::kCircuitIdentity, {TextureID::kCircuit, 0, 0, 128, 128}},
    {SpriteID::kCircuitHadamard, {TextureID::kCircuit, 128, 0, 128, 128}},
    {SpriteID::kCircuitTGate, {TextureID::kCircuit, 256, 0, 128, 128}},
    {SpriteID::kCircuitPhase, {TextureID::kCircuit, 384, 0, 128, 128}},
    {SpriteID::kCircuitPauliX, {TextureID::kCircuit, 0, 128, 128, 128}},
    {SpriteID::kCircuitPauliY, {TextureID::kCircuit, 128, 128, 128, 128}},
    {SpriteID::kCircuitPauliZ, {TextureID::kCircuit, 256, 128, 128, 128}},
    {SpriteID::kCircuitGeneric, {TextureID::kCircuit, 384, 128, 128, 128}},
    {SpriteID::kCircuitControl, {TextureID::kCircuit, 0, 256, 128, 128}},
    {SpriteID::kCircuitAntiControl, {TextureID::kCircuit, 128, 256, 128, 128}},
    {SpriteID::kCircuitSwap, {TextureID::kCircuit, 256, 256, 128, 128}},
    {SpriteID::kCircuitMeasure, {TextureID::kCircuit, 384, 256, 128, 128}},
});

class TextureManager {
private:
  static constexpr std::size_t kNumTextures = kTextureToPathMap.Size();

  std::array<Texture, kNumTextures> textures_ = {};
  std::map<TextureID, const Texture *> id_to_texture_ = {};

  bool textures_loaded_ = false;

public:
  void LoadAllTextures(WGPUDevice device);
  void UnloadAllTextures();

  [[nodiscard]] const Texture *GetTexture(TextureID id) const;

  /**
   * Load a texture from the file system
   *
   * @param path The path to load from
   * @param device The webgpu device
   * @param format Which format to use for the image
   * @return A new texture object with the texture, texture view, width, and height.
   */
  static Texture LoadTextureFromFile(const fs::path &path, WGPUDevice device,
                                     WGPUTextureFormat format = WGPUTextureFormat_RGBA8Unorm);

  ~TextureManager() { UnloadAllTextures(); }
};

#endif // TEXTURE_H
