# Create the shared library.
add_library(DebugWindow SHARED
    DebugWindow.cpp
    DebugWindowDialog.cpp
    DebugWindow.rc       # Resource file
    DebugWindow.def      # Exports symbol table
    StdAfx.cpp           # Creates the PCH
)

target_precompile_headers(DebugWindow PRIVATE
    StdAfx.h
)

target_compile_definitions(DebugWindow PRIVATE
    ${GNG_COMPILE_DEFINITIONS}

    _AFXDLL # Requiered for Dynamic Runtime
    WIN32_LEAN_AND_MEAN
)
target_compile_options(DebugWindow PRIVATE
    ${GNG_COMPILE_OPTIONS}
)
target_link_options(DebugWindow PRIVATE
    ${GNG_LINK_OPTIONS}
)

set_target_properties(DebugWindow PROPERTIES
    ${GNG_GLOBAL_PROPERTIES}

    MFC_FLAG 2
)
