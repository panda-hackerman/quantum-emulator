#define EXIT_SUCCESS 0
#define EXIT_FAILURE 1

#include <exception>
#include <iostream>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

#include "application.h"

inline std::string ExceptionWhat(
    const std::exception_ptr &e_ptr = std::current_exception()) noexcept {
  if (!e_ptr) return "[BAD EXCEPTION]";

  try {
    std::rethrow_exception(e_ptr);
  } catch (const std::exception &ex) {
    return ex.what();
  } catch (const std::string &ex) {
    return ex;
  } catch (const char *ex) {
    return ex;
  } catch (...) {
    return "[UNKNOWN EXCEPTION]";
  }
}

int main() {
  try {
    Application app;

    if (!app.Init()) {
      return EXIT_FAILURE; // <-- Breakpoint recommended here
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

    // app.Terminate(); //< This is called when app is destroyed anyway...
  } catch (...) {
    std::cerr << "Program failed due to an unhandled exception: " << ExceptionWhat() << std::endl;
    return EXIT_FAILURE; // <-- Breakpoint recommended here
  }

  return EXIT_SUCCESS;
}