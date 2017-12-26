#pragma once
#include "../Resource/Resource.h"

namespace Unique
{
	struct ImageDesc
	{
		int width;
		int height;
		int bitsPerPixel;
		int numComponents;
		int rowStride; // In bytes
	};

	class Image : public Resource
	{
		uRTTI(Image, Resource)
	public:
		Image();
		~Image();

		bool Load(IStream& file);

		static SPtr<Image> LoadImage(const char* filename);
		
		const ImageDesc& GetDesc() { return desc_; }
		byte* GetData() { return data_; }
	protected:
		ImageDesc desc_;
		int reqComponents = 0;
		byte* data_ = nullptr;
	};

	class ImageImporter : public ResourceImporter
	{
	public:

	};

	static const float a = 0.055f;

	// https://en.wikipedia.org/wiki/SRGB
	inline float SRGBToLinear(float SRGB)
	{
		if (SRGB < 0.04045f)
			return SRGB / 12.92f;
		else
			return pow((SRGB + a) / (1 + a), 2.4f);
	}

	inline float LinearToSRGB(float c)
	{
		if (c < 0.0031308f)
			return 12.92f * c;
		else
			return (1 + a) * pow(c, 1.f / 2.4f) - a;
	}

	template<typename ChannelType>
	ChannelType SRGBAverage(ChannelType c0, ChannelType c1, ChannelType c2, ChannelType c3)
	{
		const float NormVal = static_cast<float>(std::numeric_limits<ChannelType>::max());
		float fc0 = static_cast<float>(c0) / NormVal;
		float fc1 = static_cast<float>(c1) / NormVal;
		float fc2 = static_cast<float>(c2) / NormVal;
		float fc3 = static_cast<float>(c3) / NormVal;

		float fLinearAverage = (SRGBToLinear(fc0) + SRGBToLinear(fc1) + SRGBToLinear(fc2) + SRGBToLinear(fc3)) / 4.f;
		float fSRGBAverage = LinearToSRGB(fLinearAverage);
		Int32 SRGBAverage = static_cast<Int32>(fSRGBAverage * NormVal);
		SRGBAverage = std::min(SRGBAverage, static_cast<Int32>(std::numeric_limits<ChannelType>::max()));
		SRGBAverage = std::max(SRGBAverage, static_cast<Int32>(std::numeric_limits<ChannelType>::min()));
		return static_cast<ChannelType>(SRGBAverage);
	}

	template < typename ChannelType >
	void ComputeCoarseMip(uint NumChannels, bool IsSRGB,
		const void *pFineMip, uint FineMipStride,
		void *pCoarseMip, uint CoarseMipStride,
		uint CoarseMipWidth, uint CoarseMipHeight)
	{
		for (uint row = 0; row < CoarseMipHeight; ++row)
			for (uint col = 0; col < CoarseMipWidth; ++col)
			{
				auto FineRow0 = reinterpret_cast<const ChannelType*>(reinterpret_cast<const Uint8*>(pFineMip) + row * 2 * FineMipStride);
				auto FineRow1 = reinterpret_cast<const ChannelType*>(reinterpret_cast<const Uint8*>(pFineMip) + (row * 2 + 1) * FineMipStride);

				for (uint c = 0; c < NumChannels; ++c)
				{
					auto Col00 = FineRow0[col * 2 * NumChannels + c];
					auto Col01 = FineRow0[(col * 2 + 1) * NumChannels + c];
					auto Col10 = FineRow1[col * 2 * NumChannels + c];
					auto Col11 = FineRow1[(col * 2 + 1) * NumChannels + c];
					auto &DstCol = reinterpret_cast<ChannelType*>(reinterpret_cast<Uint8*>(pCoarseMip) + row * CoarseMipStride)[col * NumChannels + c];
					if (IsSRGB)
						DstCol = SRGBAverage(Col00, Col01, Col10, Col11);
					else
						DstCol = (Col00 + Col01 + Col10 + Col11) / 4;
				}
			}
	}

	template < typename ChannelType >
	void RGBToRGBA(const void *pRGBData, uint RGBStride,
		void *pRGBAData, uint RGBAStride,
		uint Width, uint Height)
	{
		for (uint row = 0; row < Height; ++row)
			for (uint col = 0; col < Width; ++col)
			{
				for (int c = 0; c < 3; ++c)
				{
					reinterpret_cast<ChannelType*>((reinterpret_cast<Uint8*>(pRGBAData) + RGBAStride * row))[col * 4 + c] =
						reinterpret_cast<const ChannelType*>((reinterpret_cast<const Uint8*>(pRGBData) + RGBStride * row))[col * 3 + c];
				}
				reinterpret_cast<ChannelType*>((reinterpret_cast<Uint8*>(pRGBAData) + RGBAStride * row))[col * 4 + 3] = std::numeric_limits<ChannelType>::max();
			}
	}
}
