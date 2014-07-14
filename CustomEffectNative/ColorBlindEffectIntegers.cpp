// ColorblindEffectIntegers.cpp
#include "pch.h"
#include <wrl.h>
#include <robuffer.h>
#include "ColorblindEffectIntegers.h"
#include <ppltasks.h>
#include "Helpers.h"

using namespace CustomEffectNative;
using namespace Platform;
using namespace concurrency;
using namespace Nokia::Graphics::Imaging;
using namespace Microsoft::WRL;

ColorblindEffectIntegers::ColorblindEffectIntegers()
{
}

Windows::Foundation::IAsyncAction^ ColorblindEffectIntegers::LoadAsync()
{
	return create_async([this]
	{
	});
}

void ColorblindEffectIntegers::Process(Windows::Foundation::Rect rect)
{
	unsigned int sourceLength, targetLength;
	byte* sourcePixels = CustomEffectNative::Helpers::GetPointerToPixelData(sourceBuffer, &sourceLength);
	byte* targetPixels = CustomEffectNative::Helpers::GetPointerToPixelData(targetBuffer, &targetLength);

	unsigned int minX = (unsigned int)rect.X * 4;
	unsigned int minY = (unsigned int)rect.Y;
	unsigned int maxX = minX + (unsigned int)rect.Width * 4;
	unsigned int maxY = minY + (unsigned int)rect.Height;

	const int MAX = 255 << 8;
	for(unsigned int y = minY; y < maxY; y++)
	{
		unsigned int xOffset = y * imageWidth * 4;
		for(unsigned int x = minX; x < maxX; x += 4) 
		{
			//Imaging SDK uses Blue, Green, Red, Alpha Image Format with 8 bits/channel
			int b = sourcePixels[xOffset + x];
			int g = sourcePixels[xOffset + x + 1];
			int r = sourcePixels[xOffset + x + 2];
			byte a = sourcePixels[xOffset + x + 3];


			int newR = 29 * b + 150 * g + 77 * r;
			int newG = -15 * b + 179 * g + 91 * r;

			if (newR > MAX) newR = MAX;

			if (newG > MAX) newG = MAX;
			if (newG < 0) newG = 0;

			targetPixels[xOffset + x] = (byte) b;
			targetPixels[xOffset + x + 1] = (byte) (newG >> 8);
			targetPixels[xOffset + x + 2] = (byte) (newR >> 8);
			targetPixels[xOffset + x + 3] = a;
		}
	}
}

Windows::Storage::Streams::IBuffer^ ColorblindEffectIntegers::ProvideSourceBuffer(Windows::Foundation::Size imageSize)
{
	unsigned int size = (unsigned int)(4 * imageSize.Height * imageSize.Width);
	sourceBuffer = ref new Windows::Storage::Streams::Buffer(size);
	sourceBuffer->Length = size;
	imageWidth = (unsigned int)imageSize.Width;
	return sourceBuffer;
}

Windows::Storage::Streams::IBuffer^ ColorblindEffectIntegers::ProvideTargetBuffer(Windows::Foundation::Size imageSize)
{
	unsigned int size = (unsigned int)(4 * imageSize.Height * imageSize.Width);
	targetBuffer = ref new Windows::Storage::Streams::Buffer(size);
	targetBuffer->Length = size;
	return targetBuffer;
}