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

set_target_properties(WWDownload PROPERTIES
    ${GNG_GLOBAL_PROPERTIES}
 )

target_include_directories(WWDownload PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/.."
)

target_compile_definitions(WWDownload PRIVATE
    ${GNG_COMPILE_DEFINITIONS}

    WIN32_LEAN_AND_MEAN
)

target_compile_options(WWDownload PRIVATE
    ${GNG_COMPILE_OPTIONS}
)

target_link_options(WWDownload PRIVATE
    ${GNG_LINK_OPTIONS}
)
