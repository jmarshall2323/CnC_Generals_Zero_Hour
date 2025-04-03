cmake_minimum_required(VERSION 3.15)

project(RTS LANGUAGES C CXX)

set(CMAKE_CONFIGURATION_TYPES "Debug;Internal;Release" CACHE STRING "Configs" FORCE)

################################################################################
# SETUP
################################################################################


set(GNG_GLOBAL_PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
)

##### AUTOMATED #####

set(GNG_COMPILE_OPTIONS "")
set(GNG_LINK_OPTIONS "")
set(GNG_COMPILE_DEFINITIONS "")

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(GNG_DEBUG TRUE)
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Internal")
    set(GNG_INTERNAL TRUE)
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Release")
    set(GNG_RELEASE TRUE)
endif()

##### Compiler & Linker Settings #####

# Microsoft Visual C/C++ Compiler
if(MSVC)

    ##### Common #####

    set(GNG_COMPILE_OPTIONS
        /nologo # Hide compiler copyright notice
        /W1     # Warnings level 1 (TODO Increase it as we fix such warnnings)
    )
    set(GNG_LINK_OPTIONS
        /NOLOGO # Hide linker copyright notice
    )
    set(GNG_COMPILE_DEFINITIONS
        _WINDOWS
        WINVER=0x0A00              # Windows 10 is the minimum as we use DX12
        _WIN32_WINNT=0x0A00        # Windows 10 is the minimum as we use DX12

        _CRT_SECURE_NO_WARNINGS
        _SILENCE_STDEXT_HASH_DEPRECATION_WARNINGS
    )

    ##### Debug #####

    if(GNG_DEBUG)
        list(APPEND GNG_COMPILE_OPTIONS
            /Od   # Disable optimizations
            /MDd  # MultiThreaded Debug DLL Runtime
            /Zi   # Generate Debug Database (.pdb)
            /RTC1 # Runtime Error Checking
        )
        list(APPEND GNG_LINK_OPTIONS
            /DEBUG
        )
        list(APPEND GNG_COMPILE_DEFINITIONS
            _DEBUG
        )

    ##### Internal #####

    elseif(GNG_INTERNAL)
        list(APPEND GNG_COMPILE_OPTIONS
            /O2 # Favor speed over size
            /MD # MultiThreaded DLL Runtime
            /Zi # Generate Debug Database (.pdb)
            /Gy # Enable Function level linking
        )
        list(APPEND GNG_LINK_OPTIONS
            /DEBUG
        )
        list(APPEND GNG_COMPILE_DEFINITIONS
            _INTERNAL
            NDEBUG
        )

    ##### Release #####

    elseif(GNG_RELEASE)
        list(APPEND GNG_COMPILE_OPTIONS
            /O2 # Favor speed over size
            /MD # MultiThreaded DLL Runtime
            /Gy # Enable Function level linking
        )
        list(APPEND GNG_LINK_OPTIONS

        )
        list(APPEND GNG_COMPILE_DEFINITIONS
            _RELEASE
            NDEBUG
            IG_DEBUG_STACKTRACE
            WIN32
        )
    endif()
endif()

################################################################################

add_subdirectory(GameEngine)
add_subdirectory(GameEngineDevice)
add_subdirectory(GameRenderer)
add_subdirectory(Tools/WorldBuilder)
add_subdirectory(Tools/DebugWindow)
add_subdirectory(Tools/ParticleEditor)
add_subdirectory(Tools/GUIEdit)
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

# Save off the current build type
set(_OLD_BUILD_TYPE "${CMAKE_BUILD_TYPE}")

# Force subdirectory to Release
set(CMAKE_BUILD_TYPE "Release")

add_subdirectory(Libraries/OpenAL)

# Restore previous build type
set(CMAKE_BUILD_TYPE "${_OLD_BUILD_TYPE}")

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
    ${GNG_GLOBAL_PROPERTIES}
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
    ${GNG_COMPILE_DEFINITIONS}
)

target_compile_options(RTS PRIVATE
    ${GNG_COMPILE_OPTIONS}
)

target_link_options(RTS PRIVATE
    ${GNG_LINK_OPTIONS}
)

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
    DebugWindow
    ParticleEditor
    GameEngine
    WinPixEventRuntime
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
    ${CMAKE_CURRENT_SOURCE_DIR}/Libraries/Pix/x64
)
