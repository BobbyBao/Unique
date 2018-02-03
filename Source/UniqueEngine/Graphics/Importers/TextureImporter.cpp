#include "Precompiled.h"
#include "TextureImporter.h"

#include "Resource/Image.h"
#include "DDSLoader.h"
#include "../Graphics.h"

namespace Unique
{
	TextureImporter::TextureImporter() : ResourceImporter(Texture::GetTypeStatic())
	{
	}

	SPtr<Resource> TextureImporter::Import(const String& filePath)
	{
		if (filePath.EndsWith(".dds", false))
		{
			return LoadDDS(filePath);
		}
		else
		{
			auto& cache = GetSubsystem<ResourceCache>();
			Image* img = cache.GetResource<Image>(filePath);
			SPtr<Texture> tex(new Texture());
			if (tex->Create(*img, TexLoadInfo))
			{
				return tex;
			}
		}

		return nullptr;
	}


	SPtr<Resource> TextureImporter::LoadDDS(const String& filePath)
	{
		ResourceCache& cache = GetSubsystem<ResourceCache>();
		SPtr<File> file = cache.GetFile(filePath);
		if (!file)
		{
			return nullptr;
		}
		
		ByteArray data = file->ReadAll();
		SPtr<Texture> texture(new Texture());

		CreateDDSTextureFromMemoryEx(
			reinterpret_cast<const byte*>(data.data()),
			data.size(),
			0, // maxSize
			TexLoadInfo.Usage,
			TexLoadInfo.Name,
			TexLoadInfo.BindFlags,
			TexLoadInfo.CPUAccessFlags,
			0, // miscFlags
			TexLoadInfo.IsSRGB, // forceSRGB
			texture);
		
		return texture;
	}

}
