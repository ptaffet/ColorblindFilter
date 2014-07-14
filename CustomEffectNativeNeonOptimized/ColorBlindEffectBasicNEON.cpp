// ColorBlindEffectBasicNEON.cpp
#include "pch.h"
#include <wrl.h>
#include <robuffer.h>
#include "ColorBlindEffectBasicNEON.h"
#include <ppltasks.h>
#include "Helpers.h"
#include <arm_neon.h>

using namespace CustomEffectNativeNeonOptimized;
using namespace Platform;
using namespace concurrency;
using namespace Nokia::Graphics::Imaging;
using namespace Microsoft::WRL;

ColorBlindEffectBasicNEON::ColorBlindEffectBasicNEON()
{
}

Windows::Foundation::IAsyncAction^ ColorBlindEffectBasicNEON::LoadAsync()
{
	return create_async([this]
	{
	});
}

void ColorBlindEffectBasicNEON::Process(Windows::Foundation::Rect rect)
{
	unsigned int sourceLength, targetLength;
	byte* sourcePixels = CustomEffectNativeNeonOptimized::Helpers::GetPointerToPixelData(sourceBuffer, &sourceLength);
	byte* targetPixels = CustomEffectNativeNeonOptimized::Helpers::GetPointerToPixelData(targetBuffer, &targetLength);

	unsigned int minX = (unsigned int)rect.X * 4;
	unsigned int minY = (unsigned int)rect.Y;
	unsigned int maxX = minX + (unsigned int)rect.Width * 4;
	unsigned int maxY = minY + (unsigned int)rect.Height;

	uint16x8_t maxes = vdupq_n_u16(0x7FFF);
	
	unsigned int iwidth4 = imageWidth * 4;
	unsigned int xOffset = minY*iwidth4;
	if ((maxX - minX) % 32 != 0)
		throw (-1);
	if ((maxX - minX) < 64 != 0)
		throw (-2);



	for (unsigned int y = minY; y < maxY; y++)
	{
		for (unsigned int x = minX; x < maxX; x += 32)
		{
			//Imaging SDK uses Blue, Green, Red, Alpha Image Format with 8 bits/channel
			// Load from memory
			uint8x8x4_t loaded = vld4_u8(&sourcePixels[xOffset + x]);
			// Expand to 16 bit integers
			uint16x8_t bin = vmovl_u8(loaded.val[0]);
			uint16x8_t gin = vmovl_u8(loaded.val[1]);
			uint16x8_t rin = vmovl_u8(loaded.val[2]);

			// Do all the multiplications
			uint16x8_t p3 = vmulq_n_u16(bin, 14);
			uint16x8_t p1 = vmulq_n_u16(gin, 90);
			uint16x8_t p4 = vmulq_n_u16(gin, 75);
			uint16x8_t p2 = vmulq_n_u16(rin, 46);
			uint16x8_t p5 = vmulq_n_u16(rin, 38);


			// Do the additions to get the final result
			uint16x8_t destr = vaddq_u16(p3, p4);
			uint16x8_t destg = vaddq_u16(p1, p2);
			destr = vaddq_u16(destr, p5);

			// Cap the values at 2^15-1
			destg = vminq_u16(destg, maxes);
			destr = vminq_u16(destr, maxes);

			// Divide by 2^7 by shifting right 7 bits
			destg = vshrq_n_u16(destg, 7);
			destr = vshrq_n_u16(destr, 7);

			// Compact to 8 bit integers
			loaded.val[1] = vmovn_u16(destg);
			loaded.val[2] = vmovn_u16(destr);

			// Store the results
			vst4_u8(&targetPixels[xOffset + x], loaded);
		}
		
		xOffset += iwidth4;
	}
}

Windows::Storage::Streams::IBuffer^ ColorBlindEffectBasicNEON::ProvideSourceBuffer(Windows::Foundation::Size imageSize)
{
	unsigned int size = (unsigned int)(4 * imageSize.Height * imageSize.Width);
	sourceBuffer = ref new Windows::Storage::Streams::Buffer(size);
	sourceBuffer->Length = size;
	imageWidth = (unsigned int)imageSize.Width;
	return sourceBuffer;
}

Windows::Storage::Streams::IBuffer^ ColorBlindEffectBasicNEON::ProvideTargetBuffer(Windows::Foundation::Size imageSize)
{
	unsigned int size = (unsigned int)(4 * imageSize.Height * imageSize.Width);
	targetBuffer = ref new Windows::Storage::Streams::Buffer(size);
	targetBuffer->Length = size;
	return targetBuffer;
}