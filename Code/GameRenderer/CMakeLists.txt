set(GAMERENDERER_SOURCES
    dx12/tinydx.cpp
    imgui/imgui.cpp
    imgui/imgui_draw.cpp
    imgui/imgui_impl_dx12.cpp
    imgui/imgui_impl_win32.cpp
    imgui/imgui_tables.cpp
    imgui/imgui_widgets.cpp
    dx8/dx8caps.cpp
    dx8/dx8fvf.cpp
    dx8/dx8indexbuffer.cpp
    dx8/dx8polygonrenderer.cpp
    dx8/dx8renderer.cpp
    dx8/dx8texman.cpp
    dx8/dx8vertexbuffer.cpp
    dx8/dx8webbrowser.cpp
    dx8/dx8wrapper.cpp
    dx8/dx8timer.cpp
    dx8/dx8rendertarget.cpp
)

set(GAMERENDERER_HEADERS
    dx12/tinydx.h
    dx8/dx8caps.h
    dx8/dx8fvf.h
    dx8/dx8indexbuffer.h
    dx8/dx8list.h
    dx8/dx8polygonrenderer.h
    dx8/dx8renderer.h
    dx8/dx8texman.h
    dx8/dx8vertexbuffer.h
    dx8/dx8webbrowser.h
    dx8/dx8wrapper.h
    GameRenderer.h
)

add_library(GameRenderer STATIC
    ${GAMERENDERER_SOURCES}
    ${GAMERENDERER_HEADERS}
)

set_target_properties(GameRenderer PROPERTIES
    ${GNG_GLOBAL_PROPERTIES}
 )

target_include_directories(GameRenderer PRIVATE
    .
    ../Libraries/DX90SDK/Include
    ../Libraries/Include
    ../Libraries/Pix/Include
    ../Libraries/Source/
    ../Libraries/Source/Compression
    ../Libraries/Source/GameSpy
    ../Libraries/Source/WWVegas
    ../Libraries/Source/WWVegas/WW3D2
    ../Libraries/Source/WWVegas/WWDebug
    ../Libraries/Source/WWVegas/WWLib
    ../Libraries/Source/WWVegas/WWMath
    ../Libraries/Source/WWVegas/WWSaveLoad
    Include
    Include/Precompiled
)

target_compile_definitions(GameRenderer PRIVATE
    ${GNG_COMPILE_DEFINITIONS}
    WIN32_LEAN_AND_MEAN
)

target_compile_options(GameRenderer PRIVATE
    ${GNG_COMPILE_OPTIONS}
)

target_link_options(GameRenderer PRIVATE
    ${GNG_LINK_OPTIONS}
)
