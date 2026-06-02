#include <iostream>
#include  <webgpu/webgpu.h>

#include "engine.h"

int main() {

  const Engine engine {5};
  std::cout << engine.getId() << std::endl;

  WGPUInstanceDescriptor descriptor = {};
  descriptor.nextInChain = nullptr;

  WGPUInstance instance = wgpuCreateInstance(&descriptor);

  if (!instance) {
    std::cerr << "Couldn't initialize WebGPU :(" << std::endl;
    return 1;
  }

  std::cout << "WGPU Instance: " << instance << std::endl ;

  wgpuInstanceRelease(instance);

  return 0;
}
