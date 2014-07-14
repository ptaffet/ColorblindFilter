// ColorblindEffect16bit.cpp
#include "pch.h"
#include <wrl.h>
#include <robuffer.h>
#include "ColorblindEffect16bit.h"
#include <ppltasks.h>
#include "Helpers.h"

using namespace CustomEffectNative;
using namespace Platform;
using namespace concurrency;
using namespace Nokia::Graphics::Imaging;
using namespace Microsoft::WRL;

ColorblindEffect16bit::ColorblindEffect16bit()
{
}

Windows::Foundation::IAsyncAction^ ColorblindEffect16bit::LoadAsync()
{
	return create_async([this]
	{
	});
}

void ColorblindEffect16bit::Process(Windows::Foundation::Rect rect)
{
	unsigned int sourceLength, targetLength;
	byte* sourcePixels = CustomEffectNative::Helpers::GetPointerToPixelData(sourceBuffer, &sourceLength);
	byte* targetPixels = CustomEffectNative::Helpers::GetPointerToPixelData(targetBuffer, &targetLength);

	unsigned int minX = (unsigned int)rect.X * 4;
	unsigned int minY = (unsigned int)rect.Y;
	unsigned int maxX = minX + (unsigned int)rect.Width * 4;
	unsigned int maxY = minY + (unsigned int)rect.Height;
	const int MAX = 255 << 7;

	for (unsigned int y = minY; y < maxY; y++)
	{
		unsigned int xOffset = y * imageWidth * 4;
		for (unsigned int x = minX; x < maxX; x += 4)
		{
			//Imaging SDK uses Blue, Green, Red, Alpha Image Format with 8 bits/channel
			byte b = sourcePixels[xOffset + x];
			byte g = sourcePixels[xOffset + x + 1];
			byte r = sourcePixels[xOffset + x + 2];
			byte a = sourcePixels[xOffset + x + 3];


			uint16_t newR = 14 * b + 75 * g + 38 * r;
			uint16_t newG = 90 * g + 46 * r;

			if (newR > MAX) newR = MAX;

			if (newG > MAX) newG = MAX;

			targetPixels[xOffset + x] = b;
			targetPixels[xOffset + x + 1] = (byte)(newG >> 7);
			targetPixels[xOffset + x + 2] = (byte)(newR >> 7);
			targetPixels[xOffset + x + 3] = a;
		}
	}
}

Windows::Storage::Streams::IBuffer^ ColorblindEffect16bit::ProvideSourceBuffer(Windows::Foundation::Size imageSize)
{
	unsigned int size = (unsigned int)(4 * imageSize.Height * imageSize.Width);
	sourceBuffer = ref new Windows::Storage::Streams::Buffer(size);
	sourceBuffer->Length = size;
	imageWidth = (unsigned int)imageSize.Width;
	return sourceBuffer;
}

Windows::Storage::Streams::IBuffer^ ColorblindEffect16bit::ProvideTargetBuffer(Windows::Foundation::Size imageSize)
{
	unsigned int size = (unsigned int)(4 * imageSize.Height * imageSize.Width);
	targetBuffer = ref new Windows::Storage::Streams::Buffer(size);
	targetBuffer->Length = size;
	return targetBuffer;
}