//
// Created by Eli Michaud on 6/4/2026.
//

#include "device_adapter_util.h"

#include <iostream>

void OnDeviceLostCallback(const WGPUDeviceLostReason reason, char const *message, void * /*data*/) {
  std::cout << "Device lost! (" << reason << ")";
  if (message) std::cout << "\n->" << message;
  std::cout << std::endl;
}

void OnDeviceErrorCallback(const wgpu::ErrorType type, char const *message) {
  std::cout << "Device error! (" << type << ")";
  if (message) std::cout << "\n->" << message;
  std::cout << std::endl;
}
