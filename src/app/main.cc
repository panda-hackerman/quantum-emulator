#include <GLFW/glfw3.h>

#include <iostream>

#include "application.h"

int main() {

  Application app;

  if (!app.Init()) {
    return EXIT_FAILURE;
  }

  while (app.ShouldContinue()) {
    app.Tick();
  }

  app.Terminate();

  return EXIT_SUCCESS;
}