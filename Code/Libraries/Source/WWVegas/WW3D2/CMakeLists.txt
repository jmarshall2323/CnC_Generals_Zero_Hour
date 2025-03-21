set(SOURCES
    aabtree.cpp
    aabtreebuilder.cpp
    agg_def.cpp
    animobj.cpp
    assetmgr.cpp
    bitmaphandler.cpp
    bmp2d.cpp
    boxrobj.cpp
    bwrender.cpp
    camera.cpp
    collect.cpp
    coltest.cpp
    composite.cpp
    dazzle.cpp
    ddsfile.cpp
    decalmsh.cpp
    decalsys.cpp
    distlod.cpp
    dynamesh.cpp
    font3d.cpp
    formconv.cpp
    FramGrab.cpp
    hanim.cpp
    hanimmgr.cpp
    hcanim.cpp
    hlod.cpp
    hmdldef.cpp
    hmorphanim.cpp
    hrawanim.cpp
    htree.cpp
    htreemgr.cpp
    intersec.cpp
    layer.cpp
    light.cpp
    lightenvironment.cpp
    line3d.cpp
    mapper.cpp
    matinfo.cpp
    matpass.cpp
    matrixmapper.cpp
    mesh.cpp
    meshbuild.cpp
    meshdam.cpp
    meshgeometry.cpp
    meshmatdesc.cpp
    meshmdl.cpp
    meshmdlio.cpp
    metalmap.cpp
    missingtexture.cpp
    motchan.cpp
    nullrobj.cpp
    part_buf.cpp
    part_emt.cpp
    part_ldr.cpp
    pivot.cpp
    pointgr.cpp
    polyinfo.cpp
    predlod.cpp
    prim_anim.cpp
    projector.cpp
    proto.cpp
    render2d.cpp
    render2dsentence.cpp
    renderobjectrecycler.cpp
    rendobj.cpp
    rinfo.cpp
    ringobj.cpp
    scene.cpp
    segline.cpp
    seglinerenderer.cpp
    shader.cpp
    shattersystem.cpp
    snappts.cpp
    sortingrenderer.cpp
    sphereobj.cpp
    statistics.cpp
    streak.cpp
    streakRender.cpp
    stripoptimizer.cpp
    surfaceclass.cpp
    texfcach.cpp
    texproject.cpp
    texture.cpp
    textureloader.cpp
    texturethumbnail.cpp
    txt.cpp
    vertmaterial.cpp
    visrasterizer.cpp
    w3dexclusionlist.cpp
    w3d_dep.cpp
    w3d_util.cpp
    ww3d.cpp
    ww3dformat.cpp
)

# -- 2. List your header files (from <ClInclude>).
#    This is optional in CMake but often nice to show in IDEs:
set(HEADERS
  
)

# -- 3. (Optional) Non-compilable files (images, .txt, .ini, etc.)
#    Typically, you only need to list source files that must be compiled;
#    but if you want them visible in your IDE, you could do something like:
#
# set(RESOURCES
#   "RequiredAssets/Dazzle.INI"
#   "RequiredAssets/ShatterAVel.tbl"
#   "RequiredAssets/ShatterPlanes0.w3d"
#   "RequiredAssets/ShatterVel.tbl"
#   "RequiredAssets/AddProjectorGradient.tga"
#   "RequiredAssets/MultProjectorGradient.tga"
#   "RequiredAssets/w3d_missing_texture.tga"
#   "MAPPERS.TXT"
#   "RenderObjectGuide.txt"
#   "State Management.txt"
#   "intersec.inl"
# )

add_library(ww3d2 STATIC
    ${SOURCES}
    ${HEADERS}
    # ${RESOURCES}  # If you want them in the project
)

set_target_properties(ww3d2 PROPERTIES
    ${GNG_GLOBAL_PROPERTIES}
 )

target_include_directories(ww3d2 PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/../../../../GameRenderer"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../../DX90SDK/Include"
    "${CMAKE_CURRENT_SOURCE_DIR}/../wwlib"
    "${CMAKE_CURRENT_SOURCE_DIR}/../wwmath"
    "${CMAKE_CURRENT_SOURCE_DIR}/../wwdebug"
    "${CMAKE_CURRENT_SOURCE_DIR}/../wwsaveload"
)

target_link_libraries(ww3d2 PRIVATE
    GameRenderer
    d3dx9
    wwlib
    wwmath
    WWDebug
    wwsaveload
)

target_compile_definitions(ww3d2 PRIVATE
    ${GNG_COMPILE_DEFINITIONS}
    WINDOWS_IGNORE_PACKING_MISMATCH
    WIN32_LEAN_AND_MEAN # Exclude some headers to speed up the build process
)

target_compile_options(ww3d2 PRIVATE
    ${GNG_COMPILE_OPTIONS}
)

target_link_options(ww3d2 PRIVATE
    ${GNG_LINK_OPTIONS}
)
