cmake_minimum_required(VERSION 3.20)
project(FaceBlur)

set(CMAKE_CXX_STANDARD 17)

# Where OpenCV is installed (vc16 = the prebuilt MSVC libraries)
set(OpenCV_DIR "C:/opencv/build/x64/vc16/lib")
find_package(OpenCV REQUIRED)

# Core vision sources, shared by the local test exe and the DLL.
set(CORE_SOURCES
    src/HaarFaceDetector.cpp
    src/FaceAnonymizer.cpp
    src/DnnFaceDetector.cpp)

# --- Local test executable: uses the C++ classes directly (for development) ---
add_executable(FaceBlur src/main.cpp ${CORE_SOURCES})
target_include_directories(FaceBlur PRIVATE src ${OpenCV_INCLUDE_DIRS})
target_compile_definitions(FaceBlur PRIVATE MODELS_DIR=\"${CMAKE_SOURCE_DIR}/models\")
target_link_libraries(FaceBlur PRIVATE ${OpenCV_LIBS})

# --- Smoke test: a tiny C++ program that calls the DLL through its C API,
#     exactly the way C# will (no OpenCV, no C++ classes -- just the C API). ---
add_executable(SmokeTest tests/smoke_test.cpp)
target_include_directories(SmokeTest PRIVATE src)
target_compile_definitions(SmokeTest PRIVATE MODELS_DIR=\"${CMAKE_SOURCE_DIR}/models\")
target_link_libraries(SmokeTest PRIVATE FaceBlurCore)

# --- Shared library (DLL): the flat C API that C# will call via P/Invoke ---
add_library(FaceBlurCore SHARED src/FaceBlurApi.cpp ${CORE_SOURCES})
target_include_directories(FaceBlurCore PRIVATE src ${OpenCV_INCLUDE_DIRS})
target_compile_definitions(FaceBlurCore PRIVATE FACEBLUR_EXPORTS)
target_link_libraries(FaceBlurCore PRIVATE ${OpenCV_LIBS})

# After building the exe, copy the OpenCV DLLs next to it so it runs.
add_custom_command(TARGET FaceBlur POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${OpenCV_DIR}/../bin/opencv_world4110$<$<CONFIG:Debug>:d>.dll"
        "$<TARGET_FILE_DIR:FaceBlur>"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${OpenCV_DIR}/../bin/opencv_videoio_ffmpeg4110_64.dll"
        "$<TARGET_FILE_DIR:FaceBlur>"
)
