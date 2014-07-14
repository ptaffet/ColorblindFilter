// ColorblindEffectSimple.h
#pragma once

namespace CustomEffectNative
{
	public ref class ColorblindEffectSimple sealed : public Nokia::Graphics::Imaging::ICustomEffect
    {
    public:
        ColorblindEffectSimple();
		virtual Windows::Foundation::IAsyncAction^ LoadAsync();
		virtual void Process(Windows::Foundation::Rect rect);
		virtual Windows::Storage::Streams::IBuffer^ ProvideSourceBuffer(Windows::Foundation::Size imageSize);
		virtual Windows::Storage::Streams::IBuffer^ ProvideTargetBuffer(Windows::Foundation::Size imageSize);
	private:
		unsigned int imageWidth;
		Windows::Storage::Streams::Buffer^ sourceBuffer;
		Windows::Storage::Streams::Buffer^ targetBuffer;
    };
}