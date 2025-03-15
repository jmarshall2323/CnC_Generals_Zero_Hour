cmake_minimum_required(VERSION 3.15)

project(RTS LANGUAGES C CXX)

set(CMAKE_CONFIGURATION_TYPES "Debug;Internal;Release" CACHE STRING "Configs" FORCE)

add_subdirectory(GameEngine)
add_subdirectory(GameEngineDevice)
add_subdirectory(GameRenderer)
add_subdirectory(Tools/WorldBuilder)
add_subdirectory(Libraries/Source/Compression)
add_subdirectory(Libraries/Source/WWVegas/ww3d2)
add_subdirectory(Libraries/Source/WWVegas/WWDebug)
add_subdirectory(Libraries/Source/WWVegas/WWDownload)
add_subdirectory(Libraries/Source/WWVegas/WWLib)
add_subdirectory(Libraries/Source/WWVegas/WWMath)
add_subdirectory(Libraries/Source/WWVegas/WWSaveLoad)

set(ALSOFT_UTILS OFF)
set(ALSOFT_NO_CONFIG_UTIL OFF)
set(ALSOFT_EXAMPLES OFF)
set(ALSOFT_INSTALL OFF)
set(ALSOFT_INSTALL_CONFIG OFF)
set(ALSOFT_INSTALL_HRTF_DATA OFF)

set(ALSOFT_INSTALL_AMBDEC_PRESETS OFF)
set(ALSOFT_INSTALL_EXAMPLES OFF)
set(ALSOFT_INSTALL_UTILS OFF)

add_subdirectory(Libraries/OpenAL)

add_subdirectory(Libraries/Source/GameSpy)
 
# Add the main executable; WIN32 for the Windows subsystem (no console).
add_executable(RTS WIN32
    # Source files
    Libraries/Bink/include/bink.c
    Libraries/Bink/include/cleanup.c
    Libraries/Bink/include/miles.c
    Main/WinMain.cpp
    Main/RTS.rc

    # (Optional) If you want to see these headers in the project tree:
    # Main/resource.h
    # Main/WinMain.h
    # etc.
)

set_target_properties(RTS PROPERTIES
    # Put all .exe files in a folder called "Run"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/../run"
    RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_CURRENT_SOURCE_DIR}/../run"
    RUNTIME_OUTPUT_DIRECTORY_INTERNAL "${CMAKE_CURRENT_SOURCE_DIR}/../run"
    RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_CURRENT_SOURCE_DIR}/../run"

    # Each config has its own output name:
    OUTPUT_NAME_DEBUG "Generals_debug"
    OUTPUT_NAME_INTERNAL "Generals_internal"
    OUTPUT_NAME_RELEASE "Generals"
)

target_include_directories(RTS PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/GameRenderer"
    "${CMAKE_CURRENT_SOURCE_DIR}/Libraries/Bink/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/Libraries/DX90SDK/Include"
    "${CMAKE_CURRENT_SOURCE_DIR}/Libraries/Include"
    "${CMAKE_CURRENT_SOURCE_DIR}/GameEngine/Include"
    "${CMAKE_CURRENT_SOURCE_DIR}/GameEngineDevice/Include"
    "${CMAKE_CURRENT_SOURCE_DIR}/Libraries/Source/WWVegas"
    "${CMAKE_CURRENT_SOURCE_DIR}/Libraries/Source/WWVegas/WWLib"
    "${CMAKE_CURRENT_SOURCE_DIR}/Libraries/Source/WWVegas/WWMath"
    "${CMAKE_CURRENT_SOURCE_DIR}/Libraries/Source/WWVegas/WWDebug"
    "${CMAKE_CURRENT_SOURCE_DIR}/Libraries/Source/WWVegas/WWSaveLoad"
    "${CMAKE_CURRENT_SOURCE_DIR}/Libraries/Source/WWVegas/WW3D2"
    "${CMAKE_CURRENT_SOURCE_DIR}/Libraries/Include/Granny"
)

target_compile_definitions(RTS PRIVATE
    __PLACEMENT_VEC_NEW_INLINE
    _WINDOWS

    # Debug
    $<$<CONFIG:Debug>:_DEBUG>
    $<$<CONFIG:Debug>:BROWSER_DEBUG>

    # Release
    $<$<CONFIG:Release>:_RELEASE>
    $<$<CONFIG:Release>:NDEBUG>
    $<$<CONFIG:Release>:_DISABLE_STRING_ANNOTATION>
    $<$<CONFIG:Release>:_DISABLE_VECTOR_ANNOTATION>
    $<$<CONFIG:Release>:IG_DEBUG_STACKTRACE>
    $<$<CONFIG:Release>:WIN32>

    # Internal
    $<$<CONFIG:Internal>:_INTERNAL>
    $<$<CONFIG:Internal>:NDEBUG>
    $<$<CONFIG:Internal>:_DISABLE_STRING_ANNOTATION>
    $<$<CONFIG:Internal>:_DISABLE_VECTOR_ANNOTATION>
)

if(MSVC)
    target_compile_options(RTS PRIVATE
        # Debug config: no optimization, debug runtime
        $<$<CONFIG:Debug>:/Od>
        $<$<CONFIG:Debug>:/MDd>

        # Release config: optimize, release runtime
        $<$<CONFIG:Release>:/O2>
        $<$<CONFIG:Release>:/MD>

        # Internal config: often “RelWithDebInfo”-style
        $<$<CONFIG:Internal>:/O2>
        $<$<CONFIG:Internal>:/MD>

        # Common warnings level
        /W3
    )
endif()

target_link_libraries(RTS PRIVATE
    # Common Windows libs
    user32
    gdi32
    shell32

    # Common DirectX
    d3d9
    d3d9on12
    d3d12
    Shcore
    dxguid
    dinput8
    odbc32
    odbccp32
    d3dx9
    dxerr
    winmm
    vfw32
    dsound
    wsock32
    imm32
    wininet

    # External Dependencies
    OpenAL::OpenAL

    # GameSpy SDK
    UniSpySDK

    GameEngine
)

#-----------------------------------------------------------------------------------
# 6) If you need to replicate the custom build steps (e.g. “versionUpdate.exe”)
#    you can use a PRE_BUILD custom command:
#-----------------------------------------------------------------------------------
# add_custom_command(
#    TARGET RTS
#    PRE_BUILD
#    COMMAND "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/versionUpdate.exe"
#            "${CMAKE_CURRENT_SOURCE_DIR}/Main/generatedVersion.h"
#    COMMAND "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/buildVersionUpdate.exe"
#            "${CMAKE_CURRENT_SOURCE_DIR}/Main/buildVersion.h"
#    COMMENT "Incrementing version numbers..."
# )

target_link_directories(RTS PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR}/Libraries/d3d9on12/bin/src/Release
	${CMAKE_CURRENT_SOURCE_DIR}/Libraries/DX90SDK/Lib/x64
)
