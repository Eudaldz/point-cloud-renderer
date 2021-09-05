cmake_minimum_required(VERSION 3.5)

project(point-cloud-render LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
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
 "includes/point_cloud.h" "src/point_cloud.cpp")

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