#pragma once

// C API boundary for the face-blur engine. Only plain C types cross this line,
// so C# (via P/Invoke) -- or any language -- can call it. The C++ classes stay
// hidden inside the DLL.

// Export when building the DLL, import when another module uses it.
#ifdef FACEBLUR_EXPORTS
#define FACEBLUR_API __declspec(dllexport)
#else
#define FACEBLUR_API __declspec(dllimport)
#endif

#ifdef __cplusplus
extern "C" {
#endif

// Opaque handle to the engine (really a pointer to an internal C++ object).
typedef void* FaceBlurHandle;

// Create the engine: load the model once and build the detector + anonymizer.
// Returns NULL on failure (e.g. the model could not be loaded).
FACEBLUR_API FaceBlurHandle FaceBlur_Create(const char* modelPath,
                                            float scoreThreshold,
                                            int mosaicBlocks);

// Process one BGR frame in place. 'data' points to height*step bytes of 8-bit
// 3-channel (BGR) pixels; 'step' is the number of bytes per row.
FACEBLUR_API void FaceBlur_ProcessBGR(FaceBlurHandle handle,
                                      unsigned char* data,
                                      int width, int height, int step);

// Change the mosaic strength live (fewer blocks = chunkier).
FACEBLUR_API void FaceBlur_SetMosaicBlocks(FaceBlurHandle handle, int blocks);

// Destroy the engine and free its resources.
FACEBLUR_API void FaceBlur_Destroy(FaceBlurHandle handle);

#ifdef __cplusplus
}
#endif
