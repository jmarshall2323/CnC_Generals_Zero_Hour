add_library(WWDownload STATIC
	Download.cpp
	Ftp.cpp
	registry.cpp
	urlBuilder.cpp

	# If you want headers listed for IDE visibility:
	# Download.h
	# DownloadDebug.h
	# ...
)

target_include_directories(WWDownload PRIVATE
	"${CMAKE_CURRENT_SOURCE_DIR}/.."
)

target_compile_definitions(WWDownload PRIVATE
	_LIB

	$<$<CONFIG:Debug>:_DEBUG>

	$<$<CONFIG:Internal>:DEBUG>
	$<$<CONFIG:Internal>:_DISABLE_STRING_ANNOTATION>
	$<$<CONFIG:Internal>:_DISABLE_VECTOR_ANNOTATION>

	$<$<CONFIG:Release>:WIN32>
	$<$<CONFIG:Release>:NDEBUG>
)

if(MSVC)
	target_compile_options(WWDownload PRIVATE
		/W3
		/Gy

		# Debug => /Od + /MDd + basic runtime checks (/RTC1 or /RTCs)
		$<$<CONFIG:Debug>:/Od>
		$<$<CONFIG:Debug>:/MDd>
		$<$<CONFIG:Debug>:/RTC1>

		# Internal
		#  (MaxSpeed => /O2, OnlyExplicitInline => /Ob1, MultiThreadedDLL => /MD)
		$<$<CONFIG:Internal>:/O2>
		$<$<CONFIG:Internal>:/Ob1>
		$<$<CONFIG:Internal>:/MD>

		# Release
		#  (MaxSpeed => /O2, AnySuitable => /Ob2, MultiThreadedDLL => /MD)
		$<$<CONFIG:Release>:/O2>
		$<$<CONFIG:Release>:/Ob2>
		$<$<CONFIG:Release>:/MD>
	)
endif()
