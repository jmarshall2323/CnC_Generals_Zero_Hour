add_library(ParticleEditor SHARED
    # Resource
    ParticleEditor.rc
    # .def file for exports
    ParticleEditor.def
    # .cpp files
    CButtonShowColor.cpp
    CColorAlphaDialog.cpp
    CSwitchesDialog.cpp
    EmissionTypePanels.cpp
    MoreParmsDialog.cpp
    ParticleEditor.cpp
    ParticleEditorDialog.cpp
    ParticleTypePanels.cpp
    ShaderTypePanels.cpp
    StdAfx.cpp  # Creates the PCH
    VelocityTypePanels.cpp
)

target_compile_definitions(ParticleEditor PRIVATE
    ${GNG_COMPILE_DEFINITIONS}

    _MBCS
    _AFXDLL # Requiered for Dynamic Runtime
    WIN32_LEAN_AND_MEAN
)
target_compile_options(ParticleEditor PRIVATE
    ${GNG_COMPILE_OPTIONS}
)
target_link_options(ParticleEditor PRIVATE
    ${GNG_LINK_OPTIONS}
)

set_target_properties(ParticleEditor PROPERTIES
    ${GNG_GLOBAL_PROPERTIES}

    MFC_FLAG 2
)

target_include_directories(ParticleEditor PRIVATE
    ./include
    ./res
    ../../libraries/include
    ../../gameengine/include
    ../../gameenginedevice/include
    ../../libraries/source/wwvegas
    ../../libraries/source/wwvegas/ww3d2
    ../../libraries/source/wwvegas/wwdebug
    ../../libraries/source/wwvegas/wwlib
    ../../libraries/source/wwvegas/wwmath
    ../../libraries/source/wwvegas/wwsaveload
)
