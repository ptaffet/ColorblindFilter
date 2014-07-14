// ColorblindEffectOptimizedFormula.cpp
#include "pch.h"
#include <wrl.h>
#include <robuffer.h>
#include "ColorblindEffectOptimizedFormula.h"
#include <ppltasks.h>
#include "Helpers.h"

using namespace CustomEffectNative;
using namespace Platform;
using namespace concurrency;
using namespace Nokia::Graphics::Imaging;
using namespace Microsoft::WRL;

ColorblindEffectOptimizedFormula::ColorblindEffectOptimizedFormula()
{
}

Windows::Foundation::IAsyncAction^ ColorblindEffectOptimizedFormula::LoadAsync()
{
	return create_async([this]
	{
	});
}

void ColorblindEffectOptimizedFormula::Process(Windows::Foundation::Rect rect)
{
	unsigned int sourceLength, targetLength;
	byte* sourcePixels = CustomEffectNative::Helpers::GetPointerToPixelData(sourceBuffer, &sourceLength);
	byte* targetPixels = CustomEffectNative::Helpers::GetPointerToPixelData(targetBuffer, &targetLength);

	unsigned int minX = (unsigned int)rect.X * 4;
	unsigned int minY = (unsigned int)rect.Y;
	unsigned int maxX = minX + (unsigned int)rect.Width * 4;
	unsigned int maxY = minY + (unsigned int)rect.Height;

	for(unsigned int y = minY; y < maxY; y++)
	{
		unsigned int xOffset = y * imageWidth * 4;
		for(unsigned int x = minX; x < maxX; x += 4) 
		{
			//Imaging SDK uses Blue, Green, Red, Alpha Image Format with 8 bits/channel
			double b = sourcePixels[xOffset + x];
			double g = sourcePixels[xOffset + x + 1];
			double r = sourcePixels[xOffset + x + 2];
			byte a = sourcePixels[xOffset + x + 3];

			double newR = 0.114*b + 0.587*g + 0.299*r;
			double newG = -0.05807*b + 0.701001*g + 0.357069*r;
			double newB = b;

			if (newR > 255) newR = 255;
			if (newR < 0) newR = 0;
			if (newG > 255) newG = 255;
			if (newG < 0) newG = 0;
			if (newB > 255) newB = 255;
			if (newB < 0) newB = 0;

			targetPixels[xOffset + x] = (byte) newB;
			targetPixels[xOffset + x + 1] = (byte) newG;
			targetPixels[xOffset + x + 2] = (byte) newR;
			targetPixels[xOffset + x + 3] = a;
		}
	}
}

Windows::Storage::Streams::IBuffer^ ColorblindEffectOptimizedFormula::ProvideSourceBuffer(Windows::Foundation::Size imageSize)
{
	unsigned int size = (unsigned int)(4 * imageSize.Height * imageSize.Width);
	sourceBuffer = ref new Windows::Storage::Streams::Buffer(size);
	sourceBuffer->Length = size;
	imageWidth = (unsigned int)imageSize.Width;
	return sourceBuffer;
}

Windows::Storage::Streams::IBuffer^ ColorblindEffectOptimizedFormula::ProvideTargetBuffer(Windows::Foundation::Size imageSize)
{
	unsigned int size = (unsigned int)(4 * imageSize.Height * imageSize.Width);
	targetBuffer = ref new Windows::Storage::Streams::Buffer(size);
	targetBuffer->Length = size;
	return targetBuffer;
}