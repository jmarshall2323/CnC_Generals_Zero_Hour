add_library(Compression STATIC
	# EAC
	EAC/btreeabout.cpp
	EAC/btreedecode.cpp
	EAC/btreeencode.cpp
	EAC/huffabout.cpp
	EAC/huffdecode.cpp
	EAC/huffencode.cpp
	EAC/refabout.cpp
	EAC/refdecode.cpp
	EAC/refencode.cpp

	# LZHCompress
	LZHCompress/CompLibSource/Huff.cpp
	LZHCompress/CompLibSource/Lz.cpp
	LZHCompress/CompLibSource/Lzhl.cpp
	LZHCompress/CompLibSource/Lzhl_tcp.cpp
	LZHCompress/NoxCompress.cpp

	# ZLib
	ZLib/adler32.c
	ZLib/compress.c
	ZLib/crc32.c
	ZLib/deflate.c
	ZLib/gzio.c
	ZLib/infblock.c
	ZLib/infcodes.c
	ZLib/inffast.c
	ZLib/inflate.c
	ZLib/inftrees.c
	ZLib/infutil.c
	ZLib/maketree.c
	ZLib/trees.c
	ZLib/uncompr.c
	ZLib/zutil.c

	# Top-level
	CompressionManager.cpp
)

target_include_directories(Compression PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR}/../../include
)

target_compile_definitions(Compression PRIVATE
	_CRT_SECURE_NO_WARNINGS

	$<$<CONFIG:Internal>:_DISABLE_STRING_ANNOTATION _DISABLE_VECTOR_ANNOTATION>
)

target_compile_options(Compression PRIVATE
	$<$<C_COMPILER_ID:MSVC>:
		# Debug => /Od (Optimization=Disabled), /MDd (MultiThreadedDebugDLL)
		$<$<CONFIG:Debug>:/Od /MDd /RTC1 /Zi>

		# Internal => /O2 (MaxSpeed), /Ob1 (OnlyExplicitInline), /MD, string pooling => /GF, function-level link => /Gy
		$<$<CONFIG:Internal>:/O2 /Ob1 /MD /GF /Gy>

		# Release => /O2 (MaxSpeed), /Ob2 (AnySuitable), /MD, string pooling => /GF, function-level link => /Gy
		$<$<CONFIG:Release>:/O2 /Ob2 /MD /GF /Gy>
	>
)
