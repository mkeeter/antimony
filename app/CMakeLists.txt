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

add_executable(Antimony
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

target_link_libraries(Antimony
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

target_include_directories(Antimony SYSTEM PRIVATE
    ${PYTHON_INCLUDE_DIRS}
    ${Boost_INCLUDE_DIRS}
    ${AUTOGEN_TARGETS_FOLDER}
)

################################################################################

set_property(TARGET Antimony PROPERTY CXX_STANDARD 11)

