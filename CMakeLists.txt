cmake_minimum_required(VERSION 3.5)

project(point-cloud-render LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")

add_subdirectory(glad)
find_package(GLFW3 REQUIRED)

add_executable(point-cloud-render main.cpp 
"includes/app_manager.h"  "src/app_manager.cpp"  
"includes/scene.h"  "src/scene.cpp"  
"includes/input.h"  "src/input.cpp"  
"includes/camera.h"  "src/camera.cpp" 
"includes/demo_scenes/demo_collection.h" 
"includes/demo_scenes/input_test.h" "src/demo_scenes/input_test.cpp"  
"includes/demo_scenes/cube_test.h"  "src/demo_scenes/cube_test.cpp"  
"includes/demo_scenes/camera_test.h" "src/demo_scenes/camera_test.cpp" 
"includes/camera_controller.h" "src/camera_controller.cpp" 
"includes/background.h" "src/background.cpp" 
"includes/demo_scenes/ortho_voxel_cloud_test.h" "src/demo_scenes/ortho_voxel_cloud_test.cpp" 
"includes/point_cloud.h" "src/point_cloud.cpp"  
"includes/resources.h" "src/resources.cpp"
"includes/point_cloud_scene.h" "src/point_cloud_scene.cpp"
"includes/shader_program.h" "src/shader_program.cpp"
"includes/shaders/point_shader.h" "src/shaders/point_shader.cpp" 
"includes/kdtree.h" "src/kdtree.cpp" 
"includes/tests/kdtree_test.h" "src/tests/kdtree_test.cpp" 
"includes/tests/test.h" "src/tests/test.cpp" "includes/point.h" "includes/shaders/splat_shader.h" "src/shaders/splat_shader.cpp" "includes/shaders/layered_splat_shader.h" "src/shaders/layered_splat_shader.cpp" "includes/search_queue.h"  "includes/tests/queue_test.h" "src/tests/queue_test.cpp" "includes/point_cloud_primitives.h" "src/point_cloud_primitives.cpp" "includes/shaders/ftb_splat_shader.h" "src/shaders/ftb_splat_shader.cpp" "includes/axial_projections.h" "src/axial_projections.cpp" "includes/shaders/point_normal_shade_shader.h" "includes/shaders/point_sphere_shade_shader.h" "src/shaders/point_sphere_shade_shader.cpp" "src/shaders/point_normal_shade_shader.cpp")

include_directories(includes)
include_directories(${GLFW3_INCLUDE_DIR})
include_directories(packages/glm)

target_link_libraries(point-cloud-render glad)
target_link_libraries(point-cloud-render ${GLFW3_LIBRARY})

if(WIN32)
    target_link_libraries(point-cloud-render opengl32)
endif()

if(UNIX AND NOT APPLE)
    find_package(OpenGL)
    target_link_libraries(point-cloud-render ${OPENGL_gl_LIBRARY})
endif()

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_directory
                       ${CMAKE_SOURCE_DIR}/resources/ $<TARGET_FILE_DIR:${PROJECT_NAME}>/resources)