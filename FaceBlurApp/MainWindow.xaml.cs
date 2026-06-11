using System;
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

        public MainWindow()
        {
            InitializeComponent();
        }

        private void StartButton_Click(object sender, RoutedEventArgs e)
        {
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
                    // Grab one frame; skip if the camera isn't ready yet.
                    if (_capture is null || !_capture.Read(frame) || frame.Empty())
                        continue;

                    // Convert to a WPF image and Freeze it so it can cross from
                    // this background thread to the UI thread safely.
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
            _captureTask?.Wait();   // wait for the loop to finish
            _captureTask = null;
            _cts?.Dispose();
            _cts = null;
            _capture?.Dispose();
            _capture = null;
        }

        protected override void OnClosed(EventArgs e)
        {
            StopCapture();          // clean shutdown when the window closes
            base.OnClosed(e);
        }
    }
}
