/*
 * Copyright © 2013 Nokia Corporation. All rights reserved.
 * Nokia and Nokia Connecting People are registered trademarks of Nokia Corporation. 
 * Other product and company names mentioned herein may be trademarks
 * or trade names of their respective owners. 
 * See LICENSE.TXT for license information.
 */

using Nokia.Graphics.Imaging;
using RealtimeFilterDemo.Resources;
using System;
using System.Collections.Generic;
using System.Threading;
using System.Threading.Tasks;
using Windows.Foundation;
using Windows.Phone.Media.Capture;
using Windows.Storage.Streams;
using System.Runtime.InteropServices.WindowsRuntime;

namespace RealtimeFilterDemo
{
    public class Effects : ICameraEffect
    {
        private PhotoCaptureDevice _photoCaptureDevice = null;
        private CameraPreviewImageSource _cameraPreviewImageSource = null;
        private FilterEffect _filterEffect = null;
        private IImageProvider _customEffect = null;
        private int _effectIndex = 0;
        private int _effectCount = 7;
        private Semaphore _semaphore = new Semaphore(1, 1);

        ICustomEffect native;
        public String EffectName { get; private set; }

        public PhotoCaptureDevice PhotoCaptureDevice
        {
            set
            {
                if (_photoCaptureDevice != value)
                {
                    while (!_semaphore.WaitOne(100)) ;

                    _photoCaptureDevice = value;

                    Initialize();

                    _semaphore.Release();
                }
            }
        }

        ~Effects()
        {
            while (!_semaphore.WaitOne(100)) ;

            Uninitialize();

            _semaphore.Release();
        }

        public async Task GetNewFrameAndApplyEffect(IBuffer frameBuffer, Size frameSize)
        {
            if (_semaphore.WaitOne(500))
            {
                _cameraPreviewImageSource.InvalidateLoad();

                var scanlineByteSize = (uint)frameSize.Width * 4; // 4 bytes per pixel in BGRA888 mode
                var bitmap = new Bitmap(frameSize, ColorMode.Bgra8888, scanlineByteSize, frameBuffer);

                if (_filterEffect != null)
                {
                    var renderer = new BitmapRenderer(_filterEffect, bitmap);
                    await renderer.RenderAsync();
                }
                else if (_customEffect != null)
                {
                    await _customEffect.PreloadAsync();
                    var renderer = new BitmapRenderer(_customEffect, bitmap);
                    await renderer.RenderAsync();
                }
                else
                {
                    Bitmap ycc = new Bitmap(frameSize, ColorMode.Ayuv4444);
                    var renderer = new BitmapRenderer(_cameraPreviewImageSource, ycc);
                    await renderer.RenderAsync();
                    var t = ycc.Buffers[0].Buffer.ToArray();
                    for (int i = 2; i < t.Length; i += 4)
                    {
                        t[i] = 128;
                    }
                    var bis = new BitmapImageSource(new Bitmap(frameSize, ColorMode.Ayuv4444, scanlineByteSize, t.AsBuffer()));
                    var renderer2 = new BitmapRenderer(bis, bitmap);
                    await renderer2.RenderAsync();
                }
                // GC.KeepAlive(native);
                _semaphore.Release();
            }
        }

        public void NextEffect()
        {
            if (_semaphore.WaitOne(500))
            {
                Uninitialize();

                _effectIndex++;

                if (_effectIndex >= _effectCount)
                {
                    _effectIndex = 0;
                }

                Initialize();

                _semaphore.Release();
            }
        }

        public void PreviousEffect()
        {
            if (_semaphore.WaitOne(500))
            {
                Uninitialize();

                _effectIndex--;

                if (_effectIndex < 0)
                {
                    _effectIndex = _effectCount - 1;
                }

                Initialize();

                _semaphore.Release();
            }
        }

        private void Uninitialize()
        {
            if (_cameraPreviewImageSource != null)
            {
                _cameraPreviewImageSource.Dispose();
                _cameraPreviewImageSource = null;
            }

            if (_filterEffect != null)
            {
                _filterEffect.Dispose();
                _filterEffect = null;
            }

            if (_customEffect != null)
            {
                if (_customEffect is CustomEffectBase)
                    ((CustomEffectBase)_customEffect).Dispose();
                _customEffect = null;
            }
        }

        private void Initialize()
        {
            var filters = new List<IFilter>();
            var nameFormat = "{0}/" + _effectCount + " - {1}";

            _cameraPreviewImageSource = new CameraPreviewImageSource(_photoCaptureDevice);



            switch (_effectIndex)
            {
                case 0:
                    {
                        EffectName = "Red-green colorblindness: simple";
                        native = new CustomEffectNative.ColorblindEffectSimple();
                        _customEffect = new DelegatingEffect(_cameraPreviewImageSource, native);
                    }
                    break;
                case 1:
                    {
                        EffectName = "Red-green colorblindness: optimized formula";
                        native = new CustomEffectNative.ColorblindEffectOptimizedFormula();
                        _customEffect = new DelegatingEffect(_cameraPreviewImageSource, native);
                    }
                    break;
                case 2:
                    {
                        EffectName = "Red-green colorblindness: 32-bit integers";
                        native = new CustomEffectNative.ColorblindEffectIntegers();
                        _customEffect = new DelegatingEffect(_cameraPreviewImageSource, native);
                    }
                    break;
                case 3:
                    {
                        EffectName = "Red-green colorblindness: 16-bit integers";
                        native = new CustomEffectNative.ColorblindEffect16bit();
                        _customEffect = new DelegatingEffect(_cameraPreviewImageSource, native);
                    }
                    break;
                case 4:
                    {
                        EffectName = "Red-green colorblindness: basic NEON";
                        native = new CustomEffectNative.ColorblindEffect16bit();
                        _customEffect = new DelegatingEffect(_cameraPreviewImageSource, native);
                    }
                    break;
                case 5:
                    {
                        EffectName = "Red-green colorblindness: reordered NEON";
                        native = new CustomEffectNative.ColorblindEffect16bit();
                        _customEffect = new DelegatingEffect(_cameraPreviewImageSource, native);
                    }
                    break;
                case 6:
                    {
                        EffectName = "Red-green colorblindness: fully optimized NEON";
                        native = new CustomEffectNativeNeonOptimized.ColorBlindEffectFinalNEON();
                        _customEffect = new DelegatingEffect(_cameraPreviewImageSource, native);
                    }
                    break;
            }

            if (filters.Count > 0)
            {
                _filterEffect = new FilterEffect(_cameraPreviewImageSource)
                {
                    Filters = filters
                };
            }
        }
    }
}