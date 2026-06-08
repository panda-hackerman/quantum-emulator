#define WEBGPU_CPP_IMPLEMENTATION

#define EXIT_SUCCESS 0
#define EXIT_FAILURE 1

#include "application.h"

int main() {

  Application app;

  if (!app.Init()) {
    return EXIT_FAILURE;
  }

  while (app.ShouldContinue()) {
    app.Tick(); //FIXME: Emscripten
  }

  app.Terminate();

  return EXIT_SUCCESS;
}