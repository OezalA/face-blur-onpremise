using System;
using System.IO;
using System.Threading;
using System.Threading.Tasks;
using System.Windows;
using OpenCvSharp;
using OpenCvSharp.WpfExtensions;

namespace FaceBlurApp
{
    public partial class MainWindow : System.Windows.Window
    {
        private VideoCapture? _capture;
        private CancellationTokenSource? _cts;
        private Task? _captureTask;
        private FaceBlurEngine? _engine;

        public MainWindow()
        {
            InitializeComponent();
        }

        private void StartButton_Click(object sender, RoutedEventArgs e)
        {
            // Create the native face-blur engine once (loads the model).
            if (_engine is null)
            {
                try
                {
                    string modelPath = Path.Combine(AppContext.BaseDirectory,
                        "models", "face_detection_yunet_2023mar.onnx");
                    _engine = new FaceBlurEngine(modelPath);
                }
                catch (Exception ex)
                {
                    StatusText.Text = "Engine error: " + ex.Message;
                    return;
                }
            }

            // Open the default camera (index 0).
            _capture = new VideoCapture(0);
            if (!_capture.IsOpened())
            {
                StatusText.Text = "Could not open the camera!";
                _capture.Dispose();
                _capture = null;
                return;
            }

            _cts = new CancellationTokenSource();
            _captureTask = Task.Run(() => CaptureLoop(_cts.Token));

            StartButton.IsEnabled = false;
            StopButton.IsEnabled = true;
            StatusText.Text = "Camera running";
        }

        private void CaptureLoop(CancellationToken token)
        {
            try
            {
                using var frame = new Mat();
                while (!token.IsCancellationRequested)
                {
                    if (_capture is null || !_capture.Read(frame) || frame.Empty())
                        continue;

                    // Blur faces in place via the native C++ engine. Zero-copy:
                    // we hand it the frame's own pixel buffer (Data + dimensions).
                    _engine?.ProcessBgr(frame.Data, frame.Cols, frame.Rows, (int)frame.Step());

                    var bitmap = frame.ToBitmapSource();
                    bitmap.Freeze();
                    Dispatcher.BeginInvoke(() => PreviewImage.Source = bitmap);
                }
            }
            catch (Exception ex)
            {
                Dispatcher.BeginInvoke(() => StatusText.Text = "Error: " + ex.Message);
            }
        }

        private void StopButton_Click(object sender, RoutedEventArgs e)
        {
            StopCapture();
            StatusText.Text = "Stopped";
            StartButton.IsEnabled = true;
            StopButton.IsEnabled = false;
        }

        private void StopCapture()
        {
            _cts?.Cancel();
            _captureTask?.Wait();
            _captureTask = null;
            _cts?.Dispose();
            _cts = null;
            _capture?.Dispose();
            _capture = null;
        }

        protected override void OnClosed(EventArgs e)
        {
            StopCapture();
            _engine?.Dispose();   // free the native engine
            _engine = null;
            base.OnClosed(e);
        }
    }
}
