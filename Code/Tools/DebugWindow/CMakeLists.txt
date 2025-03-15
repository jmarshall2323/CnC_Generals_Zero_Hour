add_library(DebugWindow SHARED
	DebugWindow.cpp
	DebugWindowDialog.cpp
	DebugWindow.rc    # Resource file
	DebugWindow.def   # Exports symbol table
	StdAfx.cpp        # Creates the PCH
)

set_property(TARGET DebugWindow PROPERTY MFC_FLAG 1)

target_precompile_headers(DebugWindow PRIVATE
	$<$<COMPILE_LANGUAGE:CXX>:stdafx.h>
)

target_compile_definitions(DebugWindow PRIVATE
	_MBCS
	WIN32
	_WINDOWS
	_USRDLL

	$<$<CONFIG:Debug>:_DEBUG>

	$<$<CONFIG:Release>:NDEBUG>
)

target_compile_options(DebugWindow PRIVATE
	$<$<C_COMPILER_ID:MSVC>:
		# Debug
		$<$<CONFIG:Debug>:
			/Od           # Disabled optimization
			/MTd          # Static runtime, debug
			/Zi           # Debug info
		>
		# Release
		$<$<CONFIG:Release>:
			/O2           # MaxSpeed optimization
			/Ob1          # "OnlyExplicitInline"
			/MT           # Static runtime
			/GF           # string pooling
			/Gy           # function-level linking
		>
	>
)

target_link_options(DebugWindow PRIVATE
	/SUBSYSTEM:WINDOWS
	/DYNAMICBASE:NO
	/MAP
)
