cmake_minimum_required(VERSION 3.5)

project(point-cloud-render LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")

add_subdirectory(glad)
find_package(GLFW3 REQUIRED)

add_executable(point-cloud-render "src/app_main.cpp"  
"includes/input.h" "src/input.cpp"
"includes/camera.h"  "src/camera.cpp" 
"includes/view_controller.h" "src/view_controller.cpp" 
"includes/background.h" "src/background.cpp" 
"includes/point_cloud.h" "src/point_cloud.cpp"  
"includes/kdtree.h" "src/kdtree.cpp" 
"includes/point.h" 
"includes/search_queue.h"  
"includes/point_cloud_primitives.h" "src/point_cloud_primitives.cpp" 
"includes/file_reader.h" "src/file_reader.cpp" 
"includes/renderer.h" 
"includes/linalg.h" "src/linalg.cpp" 
"includes/scene_engine.h" "src/scene_engine.cpp" )

include_directories(includes)
include_directories(${GLFW3_INCLUDE_DIR})
include_directories(packages/glm)

target_link_libraries(point-cloud-render glad)
target_link_libraries(point-cloud-render ${GLFW3_LIBRARY})

set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME "pointdraw")

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

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_directory
                       ${CMAKE_SOURCE_DIR}/pointclouds/ $<TARGET_FILE_DIR:${PROJECT_NAME}>/pointclouds)