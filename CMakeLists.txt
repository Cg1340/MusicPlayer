
cmake_minimum_required (VERSION 3.8)

project ("MusicPlayer")

set(SFML_DIR "D:\\SFML\\lib\\cmake\\SFML")

find_package(SFML REQUIRED COMPONENTS audio network graphics window system)


add_executable (MusicPlayer main.cpp math.h cJSON.c cJSON.h    "math.cpp" "keyboard.h")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET MusicPlayer PROPERTY CXX_STANDARD 17)
  set_property(TARGET MusicPlayer PROPERTY C_STANDARD 17)
endif()

target_link_libraries(MusicPlayer sfml-main sfml-audio sfml-network sfml-graphics sfml-window sfml-system)

add_custom_command(
        TARGET ${CMAKE_PROJECT_NAME}
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${PROJECT_SOURCE_DIR}/src"
            "$<TARGET_FILE_DIR:${CMAKE_PROJECT_NAME}>/src")
