// Helpers.h
#pragma once

namespace CustomEffectNative
{
	namespace Helpers
	{
	byte* GetPointerToPixelData(Windows::Storage::Streams::IBuffer^ pixelBuffer, unsigned int *length);
	}
}