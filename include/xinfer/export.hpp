#pragma once

#if defined(_WIN32) || defined(__CYGWIN__)
    #if defined(XINFER_EXPORTS)
        #define XINFER_API __declspec(dllexport)
    #else
        #define XINFER_API __declspec(dllimport)
    #endif
#else
    #if __GNUC__ >= 4
        #define XINFER_API __attribute__((visibility("default")))
    #else
        #define XINFER_API
    #endif
#endif