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

set_target_properties(wwmath PROPERTIES
    ${GNG_GLOBAL_PROPERTIES}
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
    ${GNG_COMPILE_DEFINITIONS}
    WIN32_LEAN_AND_MEAN

    # Debug:
    $<$<CONFIG:Debug>:G_CODE_BASE>
    $<$<CONFIG:Debug>:DIRECTX>

)

target_compile_options(wwmath PRIVATE
    ${GNG_COMPILE_OPTIONS}
)

target_link_options(wwmath PRIVATE
    ${GNG_LINK_OPTIONS}
)
