add_library(wwsaveload STATIC
	definition.cpp
	definitionfactory.cpp
	definitionfactorymgr.cpp
	definitionmgr.cpp
	parameter.cpp
	persistfactory.cpp
	pointerremap.cpp
	saveload.cpp
	saveloadstatus.cpp
	saveloadsubsystem.cpp
	twiddler.cpp
	wwsaveload.cpp

	# If you want to list headers too, just for IDE visibility:
	# definition.h
	# definitionclassids.h
	# definitionfactory.h
	# ...
)

target_include_directories(wwsaveload PRIVATE
	"${CMAKE_CURRENT_SOURCE_DIR}/../wwlib"
	"${CMAKE_CURRENT_SOURCE_DIR}/../wwdebug"
	"${CMAKE_CURRENT_SOURCE_DIR}/../wwmath"
)

target_compile_definitions(wwsaveload PRIVATE
	WINVER=0x400
	_WINDOWS

	$<$<CONFIG:Debug>:_DEBUG>

	$<$<CONFIG:DebugE>:_DEBUG>
	$<$<CONFIG:DebugE>:WWDEBUG>
	$<$<CONFIG:DebugE>:WIN32>
	$<$<CONFIG:DebugE>:PARAM_EDITING_ON>

	$<$<CONFIG:Internal>:NDEBUG>
	$<$<CONFIG:Internal>:_INTERNAL>

	$<$<CONFIG:Profile>:NDEBUG>
	$<$<CONFIG:Profile>:WWDEBUG>
	$<$<CONFIG:Profile>:WIN32>

	$<$<CONFIG:ProfileE>:NDEBUG>
	$<$<CONFIG:ProfileE>:WWDEBUG>
	$<$<CONFIG:ProfileE>:WIN32>
	$<$<CONFIG:ProfileE>:PARAM_EDITING_ON>

	$<$<CONFIG:Release>:NDEBUG>
	$<$<CONFIG:Release>:WIN32>
	$<$<CONFIG:Release>:IG_DEBUG_STACKTRACE>
)

#
# --- Example: Match the different MSVC runtime libraries (/MDd vs /MD),
#     optimization settings (/Od vs /O2), etc.
#
#     NOTE: In your .vcxproj:
#       - Debug, DebugE => MultiThreadedDebugDLL => /MDd
#       - Internal, Profile, ProfileE, Release => MultiThreadedDLL => /MD
#       - DebugE, ProfileE => Optimization = Disabled => /Od
#       - Debug => "MaxSpeed" => typically /O2, "InlineFunctionExpansion=AnySuitable" => /Ob2
#       - Internal => "MaxSpeed", "OnlyExplicitInline" => /O2 /Ob1
#       - Profile => "MaxSpeed", "AnySuitable" => /O2 /Ob2
#       - Release => "MaxSpeed", "AnySuitable" => /O2 /Ob2
#
