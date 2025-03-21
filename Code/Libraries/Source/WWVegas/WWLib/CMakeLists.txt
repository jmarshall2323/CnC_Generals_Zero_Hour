add_library(wwlib STATIC
    b64pipe.cpp
    b64straw.cpp
    base64.cpp
    blit.cpp
    blowfish.cpp
    buff.cpp
    bufffile.cpp
    chunkio.cpp
    convert.cpp
    cpudetect.cpp
    crc.cpp
    cstraw.cpp
    data.cpp
    ddraw.cpp
    dsurface.cpp
    ffactory.cpp
    gcd_lcm.cpp
    hash.cpp
    hsv.cpp
    ini.cpp
    int.cpp
    jshell.cpp
    keyboard.cpp
    LaunchWeb.cpp
    lcw.cpp
    load.cpp
    mono.cpp
    mpmath.cpp
    mpu.cpp
    msgloop.cpp
    multilist.cpp
    mutex.cpp
    nstrdup.cpp
    palette.cpp
    pcx.cpp
    pipe.cpp
    pk.cpp
    ramfile.cpp
    random.cpp
    rawfile.cpp
    rc4.cpp
    rcfile.cpp
    readline.cpp
    realcrc.cpp
    refcount.cpp
    registry.cpp
    rgb.cpp
    rle.cpp
    rndstrng.cpp
    sampler.cpp
    sha.cpp
    slnode.cpp
    srandom.cpp
    stimer.cpp
    straw.cpp
    strtok_r.cpp
    systimer.cpp
    tagblock.cpp
    TARGA.cpp
    textfile.cpp
    thread.cpp
    trim.cpp
    vector.cpp
    widestring.cpp
    win.cpp
    WWCOMUtil.cpp
    wwfile.cpp
    wwfont.cpp
    wwstring.cpp
    xpipe.cpp
    xstraw.cpp
    xsurface.cpp
    _convert.cpp
    _mono.cpp

    # (Optional) If you want headers visible in IDEs:
    # always.h
    # argv.h
    # b64pipe.h
    # ...
    # xsurface.h
    # _mono.h
)

set_target_properties(wwlib PROPERTIES
    ${GNG_GLOBAL_PROPERTIES}
 )

target_include_directories(wwlib PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/../wwdebug"
)

target_compile_definitions(wwlib PRIVATE
    ${GNG_COMPILE_DEFINITIONS}
    WIN32_LEAN_AND_MEAN
    REGEX_MALLOC
    STDC_HEADERS
)

target_compile_options(wwlib PRIVATE
    ${GNG_COMPILE_OPTIONS}
)

target_link_options(wwlib PRIVATE
    ${GNG_LINK_OPTIONS}
)
