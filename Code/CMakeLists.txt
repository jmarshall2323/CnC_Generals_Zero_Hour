cmake_minimum_required(VERSION 3.30)

project(RTS LANGUAGES C CXX)

################################################################################
# SETUP
################################################################################

set(GNG_GLOBAL_PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF

    # Disable C++20 modules as we don't use them
    CXX_SCAN_FOR_MODULES OFF

    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_LIST_DIR}/../Run/"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_LIST_DIR}/../Run/"
)

##### DEVELOPMENT OPTIONS #####

# Options used for development purporses only

# Development options are not meant to be used by packagers or users,
# so we avoid exposing them by using 'set' instead of 'option'

# Default settings here should provide the best end user experince not best developer experince

# Build dependencies as Release
set(GNG_FORCE_RELEASE_BUILD_DEPENDENCIES ON)

# Enable VTUNE for scripts profiling
set(GNG_VTUNE_SCRIPT_PROFILING OFF)

# Defines "PARAM_EDITING_ON" no clue what it does but probably allows run time editing of somethings - Andrew-2E128
set(GNG_PARAM_EDITING OFF)

##### AUTOMATED #####

# Interprocedural Optimization (LTO/WPO)
message(CHECK_START "Interprocedural Optimization")

include(CMakeDependentOption)
include(CheckIPOSupported)

check_ipo_supported(RESULT GNG_IPO_SUPPORTED OUTPUT error)
cmake_dependent_option(GNG_ENABLE_IPO "Enable Interprocedural Optimization (LTO/WPO)" ON "${GNG_IPO_SUPPORTED}" OFF)

if(GNG_IPO_SUPPORTED)
    if (GNG_ENABLE_IPO)
        message(CHECK_PASS " Supported and Enabled")
    else()
        message(CHECK_PASS " Supported but Disabled")
    endif()
else()
    message(CHECK_FAIL " Not Supported")
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(GNG_DEBUG TRUE)
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
    set(GNG_RELEASE TRUE)
elseif(CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
    set(GNG_RELEASE TRUE)
elseif(CMAKE_BUILD_TYPE STREQUAL "MinSizeRel")
    set(GNG_RELEASE TRUE)
endif()

if(GNG_DEBUG)
    set(GNG_ENABLE_IPO OFF)
    message(STATUS "Interprocedural Optimization disabled for debug build to reduce compile time.")
endif()

list(APPEND GNG_GLOBAL_PROPERTIES
    $<$<BOOL:${GNG_ENABLE_IPO}>:INTERPROCEDURAL_OPTIMIZATION ON>
)

##### Compiler & Linker Settings #####

# Compiler Independent options

set(GNG_COMPILE_OPTIONS
)
set(GNG_LINK_OPTIONS
)
set(GNG_COMPILE_DEFINITIONS
    $<$<BOOL:${GNG_VTUNE_SCRIPT_PROFILING}>:
        DO_VTUNE_STUFF
        SPECIAL_SCRIPT_PROFILING
    >

    $<$<BOOL:${GNG_PARAM_EDITING}>:PARAM_EDITING_ON>
)

# Microsoft Visual C/C++ Compiler
if(MSVC)

    ##### Common #####

    list(APPEND GNG_COMPILE_OPTIONS
        /W1     # Warnings level 1 (TODO Increase it as we fix such warnnings)
    )
    list(APPEND GNG_LINK_OPTIONS
    )
    list(APPEND GNG_COMPILE_DEFINITIONS
        WINVER=0x0A00              # Windows 10 is the minimum as we use DX12
        _WIN32_WINNT=0x0A00        # Windows 10 is the minimum as we use DX12

        _CRT_SECURE_NO_WARNINGS
        _SILENCE_STDEXT_HASH_DEPRECATION_WARNINGS
    )

    ##### Debug #####

    if(GNG_DEBUG)
        list(APPEND GNG_COMPILE_OPTIONS
            #/Od   # Disable optimizations [Removal Candidate: CMake default]
            #/MDd  # MultiThreaded Debug DLL Runtime [Removal Candidate: CMake default]
            #/Zi   # Generate Debug Database (.pdb) [Removal Candidate: CMake default]
            #/RTC1 # Runtime Error Checking [Removal Candidate: CMake default]
        )
        list(APPEND GNG_LINK_OPTIONS
            #/DEBUG [Removal Candidate: CMake default]
        )
        list(APPEND GNG_COMPILE_DEFINITIONS
            _DEBUG
        )

    ##### Release #####

    elseif(GNG_RELEASE)
        list(APPEND GNG_COMPILE_OPTIONS
            #/O2 # Favor speed over size [Removal Candidate: CMake default]
            #/MD # MultiThreaded DLL Runtime [Removal Candidate: CMake default]
            #/Gy # Enable Function level linking [Removal Candidate: Does not seem to have any effect on the final binary]
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
message(STATUS "========== GNG External Dependencies ==========")

if(GNG_FORCE_RELEASE_BUILD_DEPENDENCIES)
    # Save off the current build type
    set(_OLD_BUILD_TYPE "${CMAKE_BUILD_TYPE}")
    # Force subdirectory to Release
    set(CMAKE_BUILD_TYPE "Release")
endif()

message(STATUS "---------- OpenAL ----------")

# Disable unused features
set(ALSOFT_BACKEND_WAVE OFF)
set(ALSOFT_EAX OFF)

# Disable extras
set(ALSOFT_UTILS OFF)
set(ALSOFT_NO_CONFIG_UTIL OFF)
set(ALSOFT_EXAMPLES OFF)

# Disable all installs
set(ALSOFT_INSTALL OFF)
set(ALSOFT_INSTALL_CONFIG OFF)
set(ALSOFT_INSTALL_HRTF_DATA OFF)
set(ALSOFT_INSTALL_AMBDEC_PRESETS OFF)
set(ALSOFT_INSTALL_EXAMPLES OFF)
set(ALSOFT_INSTALL_UTILS OFF)

add_subdirectory(Libraries/OpenAL)

set_target_properties(OpenAL PROPERTIES
    # OpenAL DLL output path
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/../Run"

    $<$<BOOL:${GNG_ENABLE_IPO}>:INTERPROCEDURAL_OPTIMIZATION ON>
)

message(STATUS "---------- UniSpySDK ----------")

add_subdirectory(Libraries/Source/GameSpy)
 
set_target_properties(UniSpySDK PROPERTIES
    $<$<BOOL:${GNG_ENABLE_IPO}>:INTERPROCEDURAL_OPTIMIZATION ON>
)

if(GNG_FORCE_RELEASE_BUILD_DEPENDENCIES)
    # Restore previous build type
    set(CMAKE_BUILD_TYPE "${_OLD_BUILD_TYPE}")
endif()

message(STATUS "========== GNG ==========")
message(STATUS "Note: \"not found\" things above are required")

add_subdirectory(Libraries/Source/Compression)
add_subdirectory(Libraries/Source/WWVegas/ww3d2)
add_subdirectory(Libraries/Source/WWVegas/WWDebug)
add_subdirectory(Libraries/Source/WWVegas/WWDownload)
add_subdirectory(Libraries/Source/WWVegas/WWLib)
add_subdirectory(Libraries/Source/WWVegas/WWMath)
add_subdirectory(Libraries/Source/WWVegas/WWSaveLoad)

add_subdirectory(GameEngineDevice)
add_subdirectory(GameRenderer)
add_subdirectory(GameEngine)

add_subdirectory(Tools/WorldBuilder)
add_subdirectory(Tools/DebugWindow)
add_subdirectory(Tools/ParticleEditor)
add_subdirectory(Tools/GUIEdit)

# Add the main executable; WIN32 for the Windows subsystem (no console).
add_executable(RTS WIN32
    # Source files
    Libraries/Bink/include/bink.c
    Libraries/Bink/include/cleanup.c
    Libraries/Bink/include/miles.c
    Main/WinMain.cpp
    Main/RTS.rc
)

set_target_properties(RTS PROPERTIES
    ${GNG_GLOBAL_PROPERTIES}

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
