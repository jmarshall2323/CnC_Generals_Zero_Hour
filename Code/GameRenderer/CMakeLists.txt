set(GAMERENDERER_SOURCES
	dx12/tinydx.cpp
	imgui/imgui.cpp
	imgui/imgui_draw.cpp
	imgui/imgui_impl_dx9.cpp
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

target_include_directories(GameRenderer PRIVATE
	.
	../Libraries/DX90SDK/Include
	../Libraries/Include
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
	__PLACEMENT_VEC_NEW_INLINE
	_LIB
	_WINDOWS
	WIN32_LEAN_AND_MEAN
)

# Debug
target_compile_definitions(GameRenderer PRIVATE
	$<$<CONFIG:Debug>:_DEBUG>
)

# Internal
target_compile_definitions(GameRenderer PRIVATE
	$<$<CONFIG:Internal>:NDEBUG>
	$<$<CONFIG:Internal>:_INTERNAL>
	$<$<CONFIG:Internal>:_DISABLE_STRING_ANNOTATION>
	$<$<CONFIG:Internal>:_DISABLE_VECTOR_ANNOTATION>
)

# Release
target_compile_definitions(GameRenderer PRIVATE
	$<$<CONFIG:Release>:NDEBUG>
	$<$<CONFIG:Release>:IG_DEBUG_STACKTRACE>
)

if(MSVC)
	target_compile_options(GameRenderer PRIVATE
		# Common flags
		/W3
		/nologo

		# Debug
		$<$<CONFIG:Debug>:/MDd>
		$<$<CONFIG:Debug>:/Zi>
		$<$<CONFIG:Debug>:/Ob1>        # "Only Explicit"

		# Release
		$<$<CONFIG:Release>:/MD>
		$<$<CONFIG:Release>:/Zi>
		$<$<CONFIG:Release>:/O2>
		$<$<CONFIG:Release>:/Ob2>      # "Any Suitable"

		# Internal
		$<$<CONFIG:Internal>:/MD>
		$<$<CONFIG:Internal>:/Zi>
		$<$<CONFIG:Internal>:/O2>
		$<$<CONFIG:Internal>:/Ob1>     # "Only Explicit Inline"
	)
endif()
