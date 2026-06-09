cmake_minimum_required(VERSION 3.20)
project(FaceBlur)

set(CMAKE_CXX_STANDARD 17)

# Where OpenCV is installed (vc16 = the prebuilt MSVC libraries)
set(OpenCV_DIR "C:/opencv/build/x64/vc16/lib")
find_package(OpenCV REQUIRED)

# The executable and its source files
add_executable(FaceBlur src/main.cpp)

target_include_directories(FaceBlur PRIVATE ${OpenCV_INCLUDE_DIRS})
target_link_libraries(FaceBlur PRIVATE ${OpenCV_LIBS})

# After each build, copy the required OpenCV DLLs next to the executable
# (so it does not fail at runtime with "opencv_world...dll not found")
add_custom_command(TARGET FaceBlur POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${OpenCV_DIR}/../bin/opencv_world4110$<$<CONFIG:Debug>:d>.dll"
        "$<TARGET_FILE_DIR:FaceBlur>"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${OpenCV_DIR}/../bin/opencv_videoio_ffmpeg4110_64.dll"
        "$<TARGET_FILE_DIR:FaceBlur>"
)