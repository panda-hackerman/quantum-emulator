//
// Created by Eli Michaud on 6/9/2026.
//

#ifndef SLEEP_UTIL_H
#define SLEEP_UTIL_H
#include <chrono>
#include <thread>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

/// Sleep for the given amount of milliseconds
inline void SleepForMs(unsigned int ms) {
#ifdef __EMSCRIPTEN__
  emscripten_sleep(ms);
#else
  std::this_thread::sleep_for(std::chrono::milliseconds(ms));
#endif
}

#endif // SLEEP_UTIL_H
