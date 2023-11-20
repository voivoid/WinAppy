add_library(WinAppy
    inc/WinAppy/atom.h        src/WinAppy/atom.cpp
    inc/WinAppy/button.h      src/WinAppy/button.cpp
    inc/WinAppy/clipboard.h   src/WinAppy/clipboard.cpp
    inc/WinAppy/control.h     src/WinAppy/control.cpp
    inc/WinAppy/dc.h          src/WinAppy/dc.cpp
    inc/WinAppy/error.h       src/WinAppy/error.cpp
    inc/WinAppy/gdi.h         src/WinAppy/gdi.cpp
    inc/WinAppy/geo.h
    inc/WinAppy/log.h         src/WinAppy/log.cpp
    inc/WinAPpy/macros.h
    inc/WinAppy/msg_loop.h    src/WinAppy/msg_loop.cpp
                              src/WinAppy/scope_exit.h
                              src/stdafx.h
    inc/WinAppy/thunk.h       src/WinAppy/thunk.cpp
    inc/WinAppy/window.h      src/WinAppy/window.cpp
    inc/WinAppy/window_api.h  src/WinAppy/window_api.cpp)

target_include_directories(WinAppy PUBLIC inc PRIVATE src)
target_compile_options(WinAppy PRIVATE /MP /W4 /WX /permissive- /Zc:preprocessor)
target_compile_definitions(WinAppy PRIVATE NOMINMAX WIN32_LEAN_AND_MEAN UNICODE _UNICODE)

target_precompile_headers(WinAppy PRIVATE src/stdafx.h)