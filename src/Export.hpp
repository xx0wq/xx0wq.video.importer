#pragma once

#if defined(_WIN32) || defined(_WIN64)
  // Windows/MSVC export
  #define VI_EXPORT __declspec(dllexport)
#else
  // GCC/Clang export
  #define VI_EXPORT __attribute__((visibility("default")))
#endif