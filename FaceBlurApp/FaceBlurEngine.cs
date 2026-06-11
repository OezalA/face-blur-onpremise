using System;
using System.Runtime.InteropServices;

namespace FaceBlurApp
{
    /// <summary>
    /// Managed wrapper over the native FaceBlurCore.dll. Loads the model once
    /// and blurs frames in place. Dispose() frees the native engine.
    /// </summary>
    public sealed class FaceBlurEngine : IDisposable
    {
        private const string Dll = "FaceBlurCore";

        [DllImport(Dll, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        private static extern IntPtr FaceBlur_Create(string modelPath, float scoreThreshold, int mosaicBlocks);

        [DllImport(Dll, CallingConvention = CallingConvention.Cdecl)]
        private static extern void FaceBlur_ProcessBGR(IntPtr handle, IntPtr data, int width, int height, int step);

        [DllImport(Dll, CallingConvention = CallingConvention.Cdecl)]
        private static extern void FaceBlur_Destroy(IntPtr handle);

        private IntPtr _handle;

        public FaceBlurEngine(string modelPath, float scoreThreshold = 0.6f, int mosaicBlocks = 13)
        {
            _handle = FaceBlur_Create(modelPath, scoreThreshold, mosaicBlocks);
            if (_handle == IntPtr.Zero)
                throw new InvalidOperationException("Failed to load the face-blur model: " + modelPath);
        }

        /// <summary>Blur every detected face in a BGR pixel buffer, in place.</summary>
        public void ProcessBgr(IntPtr data, int width, int height, int step)
        {
            if (_handle == IntPtr.Zero) return;
            FaceBlur_ProcessBGR(_handle, data, width, height, step);
        }

        public void Dispose()
        {
            Free();
            GC.SuppressFinalize(this);
        }

        ~FaceBlurEngine() => Free();   // safety net if Dispose() is forgotten

        private void Free()
        {
            if (_handle != IntPtr.Zero)
            {
                FaceBlur_Destroy(_handle);
                _handle = IntPtr.Zero;
            }
        }
    }
}
