add_executable(WorldBuilder WIN32
	# Resource:
	res/WorldBuilder.rc

	src/addplayerdialog.cpp
	src/AutoEdgeOutTool.cpp
	src/BaseBuildProps.cpp
	src/BlendEdgeTool.cpp
	src/BlendMaterial.cpp
	src/BorderTool.cpp
	src/brushoptions.cpp
	src/BrushTool.cpp
	src/BuildList.cpp
	src/BuildListTool.cpp
	src/CameraOptions.cpp
	src/CButtonShowColor.cpp
	src/CellWidth.cpp
	src/CFixTeamOwnerDialog.cpp
	src/ContourOptions.cpp
	src/CUndoable.cpp
	src/DrawObject.cpp
	src/EditAction.cpp
	src/EditCondition.cpp
	src/EditCoordParameter.cpp
	src/EditGroup.cpp
	src/EditObjectParameter.cpp
	src/EditParameter.cpp
	src/EulaDialog.cpp
	src/ExportScriptsOptions.cpp
	src/EyedropperTool.cpp
	src/FeatherOptions.cpp
	src/FeatherTool.cpp
	src/FenceOptions.cpp
	src/FenceTool.cpp
	src/FloodFillTool.cpp
	src/GlobalLightOptions.cpp
	src/GroveOptions.cpp
	src/GroveTool.cpp
	src/HandScrollTool.cpp
	src/ImpassableOptions.cpp
	src/LayersList.cpp
	src/LightOptions.cpp
	src/MainFrm.cpp
	src/mapobjectprops.cpp
	src/MapPreview.cpp
	src/MapSettings.cpp
	src/MeshMoldOptions.cpp
	src/MeshMoldTool.cpp
	src/MoundOptions.cpp
	src/MoundTool.cpp
	src/MyToolbar.cpp
	src/NewHeightMap.cpp
	src/ObjectOptions.cpp
	src/ObjectPreview.cpp
	src/ObjectTool.cpp
	src/OpenMap.cpp
	src/OptionsPanel.cpp
	src/PickUnitDialog.cpp
	src/playerlistdlg.cpp
	src/PointerTool.cpp
	src/PolygonTool.cpp
	src/propedit.cpp
	src/RampOptions.cpp
	src/RampTool.cpp
	src/RoadOptions.cpp
	src/RoadTool.cpp
	src/SaveMap.cpp
	src/ScorchOptions.cpp
	src/ScorchTool.cpp
	src/ScriptActionsFalse.cpp
	src/ScriptActionsTrue.cpp
	src/ScriptConditions.cpp
	src/ScriptDialog.cpp
	src/ScriptProperties.cpp
	src/SelectMacrotexture.cpp
	src/ShadowOptions.cpp
	src/SplashScreen.cpp
	src/StdAfx.cpp
	src/TeamBehavior.cpp
	src/TeamGeneric.cpp
	src/TeamIdentity.cpp
	src/TeamReinforcement.cpp
	src/teamsdialog.cpp
	src/TerrainMaterial.cpp
	src/TerrainModal.cpp
	src/TerrainSwatches.cpp
	src/TileTool.cpp
	src/Tool.cpp
	src/WaterOptions.cpp
	src/WaterTool.cpp
	src/WaypointOptions.cpp
	src/WaypointTool.cpp
	src/WBFrameWnd.cpp
	src/WBHeightMap.cpp
	src/WBPopupSlider.cpp
	src/wbview.cpp
	src/wbview3d.cpp
	src/WHeightMapEdit.cpp
	src/WorldBuilder.cpp
	src/WorldBuilderDoc.cpp
	src/WorldBuilderView.cpp
)

set_property(TARGET WorldBuilder PROPERTY
	# MFC_FLAG = 2 => Use MFC in a Shared DLL
	MFC_FLAG 2
)

target_include_directories(WorldBuilder PRIVATE
	../../GameRenderer/
	../../Libraries/OpenAL/include
	../../Libraries/Source/Compression
	../../gameengine/include
	../../gameenginedevice/include
	../../libraries/include
	../../libraries/source/wwvegas
	../../libraries/source/wwvegas/ww3d2
	../../libraries/source/wwvegas/wwdebug
	../../libraries/source/wwvegas/wwlib
	../../libraries/source/wwvegas/wwmath
	../../libraries/source/wwvegas/wwsaveload
	./res
	./include
	.
)

target_compile_definitions(WorldBuilder PRIVATE
	_AFXDLL
	_MBCS
	__PLACEMENT_VEC_NEW_INLINE
	WIN32
	_WINDOWS
	EDITOR
	REGEX_MALLOC
	STDC_HEADERS
	WIN32_LEAN_AND_MEAN

	# Debug
	$<$<CONFIG:Debug>:_DEBUG>

	# Internal
	$<$<CONFIG:Internal>:
		_DISABLE_STRING_ANNOTATION
		_DISABLE_VECTOR_ANNOTATION
		NDEBUG
		_INTERNAL
	>

	# Release
	$<$<CONFIG:Release>:
		_DISABLE_STRING_ANNOTATION
		_DISABLE_VECTOR_ANNOTATION
		IG_DEBUG_STACKTRACE
		NDEBUG
		_RELEASE
	>
)

# Typical MSVC flags for each config:
target_compile_options(WorldBuilder PRIVATE
	$<$<C_COMPILER_ID:MSVC>:

		$<$<CONFIG:Debug>:/Od /MDd /Zi /RTC1>

		$<$<CONFIG:Internal>:/O2 /Ob1 /MD /Zi /GF /Gy>

		$<$<CONFIG:Release>:/O2 /Ob2 /MD /GF /Gy>
	>
)

target_link_libraries(WorldBuilder PRIVATE
	# Common DirectX
	d3d12
	dxguid
	dinput8
	ddraw
	d3dx9
	d3d9
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

target_link_options(WorldBuilder PRIVATE
	/NODEFAULTLIB:libcd
	/DYNAMICBASE:NO
	/FORCE
)

target_link_directories(WorldBuilder PRIVATE
	../../Libraries/DX90SDK/Lib/x64
)

set_target_properties(WorldBuilder PROPERTIES
	# Put all .exe files in a folder called "Run"
	RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/../../../run"
	RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_CURRENT_SOURCE_DIR}/../../../run"
	RUNTIME_OUTPUT_DIRECTORY_INTERNAL "${CMAKE_CURRENT_SOURCE_DIR}/../../../run"
	RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_CURRENT_SOURCE_DIR}/../../../run"

	# Each config has its own output name:
	OUTPUT_NAME_DEBUG "WorldBuilder_debug"
	OUTPUT_NAME_INTERNAL "WorldBuilder_internal"
	OUTPUT_NAME_RELEASE "WorldBuilder"
)
