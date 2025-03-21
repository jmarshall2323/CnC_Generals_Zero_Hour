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

set_target_properties(WWDebug PROPERTIES
    ${GNG_GLOBAL_PROPERTIES}
 )

target_include_directories(WWDebug PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/../wwlib"
)

target_link_libraries(WWDebug PRIVATE
    wwlib
)

target_compile_definitions(WWDebug PRIVATE
    ${GNG_COMPILE_DEFINITIONS}
    WIN32_LEAN_AND_MEAN
)

target_compile_options(WWDebug PRIVATE
    ${GNG_COMPILE_OPTIONS}
)

target_link_options(WWDebug PRIVATE
    ${GNG_LINK_OPTIONS}
)
