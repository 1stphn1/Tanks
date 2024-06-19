cmake_minimum_required(VERSION 3.20)

project(Tanks)

file(COPY ${CMAKE_SOURCE_DIR}/assets DESTINATION ${CMAKE_BINARY_DIR}/bin)

add_subdirectory(dependencies/SDL2)
add_subdirectory(dependencies/SDL2_image)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

include_directories(${PROJECT_NAME} ${SDL2_INCLUDE_DIRS} ${SDL2_IMAGE_INCLUDE_DIRS})

add_executable(${PROJECT_NAME} src/main.cpp src/Game.cpp src/Player.cpp)
target_link_libraries(${PROJECT_NAME} SDL2::SDL2 SDL2::SDL2main SDL2_image::SDL2_image)

if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /W4 /WX)
else()
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif()

# copy the shared libary files to the same folder as the executable
add_custom_command(
    TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    $<TARGET_FILE:SDL2::SDL2>
    $<TARGET_FILE_DIR:${PROJECT_NAME}>
    VERBATIM
)

add_custom_command(
    TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    $<TARGET_FILE:SDL2_image::SDL2_image>
    $<TARGET_FILE_DIR:${PROJECT_NAME}>
    VERBATIM
)
