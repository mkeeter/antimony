# Instruct CMake to run moc, uic, and rrc automatically when needed.
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# Make the build directory an include target (for generated moc_ files)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Find the Qt libraries
find_package(Qt5Core REQUIRED)
find_package(Qt5Widgets REQUIRED)
find_package(Qt5OpenGL REQUIRED)
find_package(Qt5Network REQUIRED)
find_package(Qt5Concurrent REQUIRED)

set(SRC_FILES
    app/main.cpp
    app/app.cpp
    app/update.cpp
    app/colors.cpp
    canvas/scene.cpp
    canvas/canvas_view.cpp
    canvas/info.cpp
    canvas/inspector/frame.cpp
    canvas/inspector/export.cpp
    canvas/inspector/title.cpp
    canvas/inspector/buttons.cpp
    canvas/inspector/util.cpp
    canvas/datum_row.cpp
    canvas/datum_editor.cpp
    canvas/datum_port.cpp
    canvas/subdatum/subdatum_frame.cpp
    canvas/subdatum/subdatum_editor.cpp
    canvas/subdatum/subdatum_row.cpp
    canvas/connection/base.cpp
    canvas/connection/connection.cpp
    canvas/connection/dummy.cpp
    window/base.cpp
    window/canvas.cpp
    window/viewport.cpp
    window/quad.cpp
    window/base_viewport_window.cpp
    window/script_window.cpp
    graph/constructor/populate.cpp
    graph/proxy/graph.cpp
    graph/proxy/node.cpp
    graph/proxy/script.cpp
    graph/proxy/datum.cpp
    graph/proxy/subdatum.cpp
    graph/proxy/base_datum.cpp
    graph/hooks/hooks.cpp
    graph/hooks/export.cpp
    graph/hooks/ui.cpp
    graph/hooks/title.cpp
    graph/serialize/serializer.cpp
    graph/serialize/deserializer.cpp
    script/syntax.cpp
    script/editor.cpp
    script/frame.cpp
    undo/undo_command.cpp
    undo/undo_add_node.cpp
    undo/undo_add_datum.cpp
    undo/undo_delete_datum.cpp
    undo/undo_delete_multi.cpp
    undo/undo_delete_node.cpp
    undo/undo_delete_link.cpp
    undo/undo_move_node.cpp
    undo/undo_move_datum.cpp
    undo/undo_change_script.cpp
    undo/undo_change_expr.cpp
    undo/undo_stack.cpp
    dialog/exporting.cpp
    dialog/resolution.cpp
    export/export_mesh.cpp
    export/export_heightmap.cpp
    export/export_worker.cpp
    viewport/scene.cpp
    viewport/gl.cpp
    viewport/view.cpp
    viewport/image.cpp
    viewport/control/control.cpp
    viewport/control/control_instance.cpp
    viewport/control/point.cpp
    viewport/control/wireframe.cpp
    viewport/render/instance.cpp
    viewport/render/task.cpp

    gl/gl.qrc
)

set(PY_FAB_FILES
    ../py/fab/__init__.py
    ../py/fab/shapes.py
    ../py/fab/types.py
)

if (APPLE)
    set(ANTIMONY_APP Antimony)
    set(CMAKE_MACOSX_RPATH ON)

    # Add the icon file and deploy it to Resources
    set(SB_ICON ../deploy/mac/sb.icns)
    set_source_files_properties(${SB_ICON}
        PROPERTIES MACOSX_PACKAGE_LOCATION "Resources"
    )

    # Deploy fab module's files into Resources/fab
    set_source_files_properties(${PY_FAB_FILES}
        PROPERTIES MACOSX_PACKAGE_LOCATION "Resources/fab"
    )

    add_executable(${ANTIMONY_APP} MACOSX_BUNDLE
        ${SRC_FILES}
        ${SB_ICON}
        ${PY_FAB_FILES}
    )

    # Set application Info.plist file
    set_target_properties(${ANTIMONY_APP} PROPERTIES
        MACOSX_BUNDLE_INFO_PLIST ../deploy/mac/Info.plist
    )

    # Copy node directory into application bundle
    add_custom_command(TARGET ${ANTIMONY_APP} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_SOURCE_DIR}/py/nodes
            ${CMAKE_CURRENT_BINARY_DIR}/${ANTIMONY_APP}.app/Contents/Resources/nodes)
else()
    set(ANTIMONY_APP antimony)
    add_executable(${ANTIMONY_APP} ${SRC_FILES})

    # Copy node and fab directory into build directory
    add_custom_command(TARGET ${ANTIMONY_APP} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_SOURCE_DIR}/py/nodes
            ${CMAKE_CURRENT_BINARY_DIR}/sb/nodes)
    add_custom_command(TARGET ${ANTIMONY_APP} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_SOURCE_DIR}/py/fab
            ${CMAKE_CURRENT_BINARY_DIR}/sb/fab)

    install(TARGETS antimony
            DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)
    install(DIRECTORY ../py/nodes
            DESTINATION ${CMAKE_INSTALL_PREFIX}/share/antimony)
    install(DIRECTORY ../py/fab
            DESTINATION ${CMAKE_INSTALL_PREFIX}/share/antimony)
endif()

target_link_libraries(${ANTIMONY_APP}
    Qt5::Widgets
    Qt5::Gui
    Qt5::OpenGL
    Qt5::Network
    Qt5::Concurrent
    SbGraph
    SbFab)

################################################################################

execute_process(COMMAND git log --pretty=format:'%h' -n 1
                OUTPUT_VARIABLE GITREV)
execute_process(COMMAND bash -c "git diff --quiet --exit-code || echo +"
                OUTPUT_VARIABLE GITDIFF)
execute_process(COMMAND git describe --exact-match --tags
                OUTPUT_VARIABLE GITTAG
                ERROR_QUIET)
execute_process(COMMAND git rev-parse --abbrev-ref HEAD
                OUTPUT_VARIABLE GITBRANCH)

add_definitions(-D'GITREV="${GITREV}${GITDIFF}"'
                -D'GITTAG="${GITTAG}"'
                -D'GITBRANCH="${GITBRANCH}"')

################################################################################

target_include_directories(${ANTIMONY_APP} SYSTEM PRIVATE
    ${Python_INCLUDE_DIRS}
    ${Boost_INCLUDE_DIRS}
    ${AUTOGEN_TARGETS_FOLDER}
)

################################################################################

set_property(TARGET ${ANTIMONY_APP} PROPERTY CXX_STANDARD 11)
set_property(TARGET ${ANTIMONY_APP} PROPERTY C_STANDARD 99)

