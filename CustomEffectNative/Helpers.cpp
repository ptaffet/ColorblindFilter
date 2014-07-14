// Helpers.cpp
#include "pch.h"
#include <wrl.h>
#include <robuffer.h>
#include "Helpers.h"
#include <ppltasks.h>

using namespace Microsoft::WRL;

//method definition from http://msdn.microsoft.com/en-us/library/Windows/Apps/dn182761.aspx
byte* CustomEffectNative::Helpers::GetPointerToPixelData(Windows::Storage::Streams::IBuffer^ pixelBuffer, unsigned int *length)
{
	if (length != nullptr)
    {
        *length = pixelBuffer->Length;
    }
    // Query the IBufferByteAccess interface.
	ComPtr<Windows::Storage::Streams::IBufferByteAccess> bufferByteAccess;
    reinterpret_cast<IInspectable*>( pixelBuffer)->QueryInterface(IID_PPV_ARGS(&bufferByteAccess));

    // Retrieve the buffer data.
    byte* pixels = nullptr;
    bufferByteAccess->Buffer(&pixels);
    return pixels;
}