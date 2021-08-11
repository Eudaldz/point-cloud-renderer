project(glad LANGUAGES C)
add_library(glad "glad.c" "glad/glad.h" "KHR/khrplatform.h")
target_include_directories (glad PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})