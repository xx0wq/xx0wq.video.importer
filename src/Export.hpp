#pragma once

#if defined(_WIN32) || defined(_WIN64)
  #define VI_EXPORT __declspec(dllexport)
#else
  #define VI_EXPORT __attribute__((visibility("default")))
#endif