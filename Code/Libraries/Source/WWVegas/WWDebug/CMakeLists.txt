set(SOURCES
	wwdebug.cpp
	wwmemlog.cpp
	wwprofile.cpp
)

set(HEADERS
	wwdebug.h
	wwhack.h
	wwmemlog.h
	wwprofile.h
)

add_library(WWDebug STATIC
	${SOURCES}
	${HEADERS}
)

target_include_directories(WWDebug PRIVATE
	"${CMAKE_CURRENT_SOURCE_DIR}/../wwlib"
)

target_link_libraries(WWDebug
	PRIVATE
	wwlib
)

target_compile_definitions(WWDebug PRIVATE
	WINVER=0x400
	_WINDOWS
	WIN32_LEAN_AND_MEAN

	# Debug
	$<$<CONFIG:Debug>:_DEBUG>

	# Internal
	$<$<CONFIG:Internal>:NDEBUG>
	$<$<CONFIG:Internal>:_INTERNAL>

	# Profile
	$<$<CONFIG:Profile>:NDEBUG>
	$<$<CONFIG:Profile>:WWDEBUG>
	$<$<CONFIG:Profile>:WIN32>

	# Release
	$<$<CONFIG:Release>:NDEBUG>
	$<$<CONFIG:Release>:WIN32>
	$<$<CONFIG:Release>:IG_DEBUG_STACKTRACE>
)

#
# (Optional) If you want to mimic MSVC-specific compiler switches like:
#   /O2 (MaxSpeed), /Ob2 (inline any suitable), /GF (string pooling), /Gy (function-level linking),
#   and treat warnings as errors (/WX), you can do something like:
#
# if(MSVC)
#   target_compile_options(WWDebug PRIVATE
#       "$<$<CONFIG:Debug>:/O2 /Ob2 /GF /Gy /WX>"
#       "$<$<CONFIG:Internal>:/O2 /Ob1 /GF /Gy /WX>"
#       "$<$<CONFIG:Profile>:/O2 /Ob2 /GF /Gy /WX>"
#       "$<$<CONFIG:Release>:/O2 /Ob2 /GF /Gy /WX>"
#   )
# endif()
#
