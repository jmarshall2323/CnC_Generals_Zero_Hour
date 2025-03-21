add_executable(GUIEdit WIN32
    # Resource
    Resource/GUIEdit.rc

    # Main .cpp files from "Source\Dialog Procedures" etc.
    "Source/Dialog Procedures/CallbackEditor.cpp"
    "Source/Dialog Procedures/CheckBoxProperties.cpp"
    "Source/Dialog Procedures/ColorDialog.cpp"
    "Source/Dialog Procedures/ComboBoxProperties.cpp"
    "Source/Dialog Procedures/GenericProperties.cpp"
    "Source/Dialog Procedures/GridSettings.cpp"
    "Source/Dialog Procedures/ListboxProperties.cpp"
    "Source/Dialog Procedures/NewLayoutDialog.cpp"
    "Source/Dialog Procedures/ProgressBarProperties.cpp"
    "Source/Dialog Procedures/PushButtonProperties.cpp"
    "Source/Dialog Procedures/RadioButtonProperties.cpp"
    "Source/Dialog Procedures/SliderProperties.cpp"
    "Source/Dialog Procedures/StaticTextProperties.cpp"
    "Source/Dialog Procedures/TabControlProperties.cpp"
    "Source/Dialog Procedures/TextEntryProperties.cpp"
    Source/EditWindow.cpp
    Source/GUIEdit.cpp
    Source/GUIEditDisplay.cpp
    Source/GUIEditWindowManager.cpp
    Source/HierarchyView.cpp
    Source/LayoutScheme.cpp
    Source/Properties.cpp
    Source/Save.cpp
    Source/WinMain.cpp
)

target_include_directories(GUIEdit PRIVATE
    ./Include
    ./Resource
    ../../Libraries/Include
    ../../GameEngine/Include
    ../../GameEngineDevice/Include
    ../../Libraries/Source/WWVegas
    ../../Libraries/Source/WWVegas/WWLib
    ../../Libraries/Source/WWVegas/WWMath
    ../../Libraries/Source/WWVegas/WWDebug
    ../../Libraries/Source/WWVegas/WWSaveLoad
    ../../Libraries/Source/WWVegas/WW3D2
)

set_target_properties(GUIEdit PROPERTIES
    ${GNG_GLOBAL_PROPERTIES}

    # Each config has its own output name:
    OUTPUT_NAME_DEBUG "GUIEdit_debug"
    OUTPUT_NAME_INTERNAL "GUIEdit_internal"
    OUTPUT_NAME_RELEASE "GUIEdit"
)

# Apply your pre-defined compile definitions, options, and link options.
target_compile_definitions(GUIEdit PRIVATE
    ${GNG_COMPILE_DEFINITIONS}
)
target_compile_options(GUIEdit PRIVATE
    ${GNG_COMPILE_OPTIONS}
)
target_link_options(GUIEdit PRIVATE
    ${GNG_LINK_OPTIONS}
)

target_link_libraries(GUIEdit PRIVATE
    GameEngine
    GameEngineDevice
    wsock32
    dxguid
    dinput8
    d3dx9
    d3d9
    odbc32
    odbccp32
    vfw32
    winmm
    dsound
    comctl32
    imm32
)

target_link_directories(GUIEdit PRIVATE
    ../../Libraries/DX90SDK/Lib/x64
)
