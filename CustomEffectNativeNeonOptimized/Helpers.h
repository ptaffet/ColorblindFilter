// Helpers.h
#pragma once

namespace CustomEffectNativeNeonOptimized
{
	namespace Helpers
	{
	byte* GetPointerToPixelData(Windows::Storage::Streams::IBuffer^ pixelBuffer, unsigned int *length);
	}
}