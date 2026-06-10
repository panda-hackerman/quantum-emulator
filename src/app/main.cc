// #define WEBGPU_CPP_IMPLEMENTATION

#define EXIT_SUCCESS 0
#define EXIT_FAILURE 1

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

#include "application.h"

int main() {

  Application app;

  if (!app.Init()) {
    return EXIT_FAILURE;
  }

#ifdef __EMSCRIPTEN__
  auto callback = [](void *app_ptr) {
    Application &app = *reinterpret_cast<Application *>(app_ptr);
    app.Tick();
  };

  emscripten_set_main_loop_arg(callback, &app, 0, true);
#else
  while (app.ShouldContinue()) {
    app.Tick();
  }
#endif

  app.Terminate();

  return EXIT_SUCCESS;
}