add_library(wwmath STATIC
	aabox.cpp
	aabtreecull.cpp
	cardinalspline.cpp
	catmullromspline.cpp
	colmath.cpp
	colmathaabox.cpp
	colmathaabtri.cpp
	colmathfrustum.cpp
	colmathline.cpp
	colmathobbobb.cpp
	colmathobbox.cpp
	colmathobbtri.cpp
	colmathplane.cpp
	colmathsphere.cpp
	cullsys.cpp
	curve.cpp
	euler.cpp
	frustum.cpp
	gridcull.cpp
	hermitespline.cpp
	lineseg.cpp
	lookuptable.cpp
	matrix3.cpp
	matrix3d.cpp
	matrix4.cpp
	obbox.cpp
	ode.cpp
	pot.cpp
	quat.cpp
	tcbspline.cpp
	tri.cpp
	v3_rnd.cpp
	vehiclecurve.cpp
	vp.cpp
	wwmath.cpp

	# If you want the headers visible in IDEs, you can list them too:
	# aabox.h
	# aabtreecull.h
	# ...
	# wwmath.h
	# wwmathids.h
)

target_include_directories(wwmath PRIVATE
	"${CMAKE_CURRENT_SOURCE_DIR}/../../../DX90SDK/Include"
	"${CMAKE_CURRENT_SOURCE_DIR}/../wwlib"
	"${CMAKE_CURRENT_SOURCE_DIR}/../wwdebug"
	"${CMAKE_CURRENT_SOURCE_DIR}/../wwsaveload"
)

target_link_libraries(wwmath PRIVATE
	d3dx9
	wwlib
	WWDebug
	wwsaveload
)

target_compile_definitions(wwmath PRIVATE
	WINVER=0x400
	_WINDOWS
	WIN32_LEAN_AND_MEAN

	# Debug:
	$<$<CONFIG:Debug>:G_CODE_BASE>
	$<$<CONFIG:Debug>:DIRECTX>
	$<$<CONFIG:Debug>:_DEBUG>

	# Internal:
	$<$<CONFIG:Internal>:NDEBUG>
	$<$<CONFIG:Internal>:_INTERNAL>
	$<$<CONFIG:Internal>:_DISABLE_STRING_ANNOTATION>
	$<$<CONFIG:Internal>:_DISABLE_VECTOR_ANNOTATION>

	# Profile:
	$<$<CONFIG:Profile>:NDEBUG>
	$<$<CONFIG:Profile>:WWDEBUG>
	$<$<CONFIG:Profile>:WIN32>

	# Release:
	$<$<CONFIG:Release>:NDEBUG>
	$<$<CONFIG:Release>:WIN32>
	$<$<CONFIG:Release>:IG_DEBUG_STACKTRACE>
)

