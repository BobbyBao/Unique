#include "Precompiled.h"
#include "TextureImporter.h"

#include "Resource/Image.h"
#include "DDSLoader.h"
#include "../Graphics.h"

namespace Unique
{
	extern IRenderDevice* renderDevice;

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
/*
		String cachedAsset = ReplaceExtension(filePath, ".ktx");
		FileSystem& fileSystem = Subsystem<FileSystem>();
		SPtr<File> file = cache.GetFile(cachedAsset);
		if (!file)
		{
			String exeFileName = "tools\\texturec.exe";
			String args;
			String sourceFile = cache.GetResourceFileName(filePath);
			String destFile = "cache/" + cachedAsset;
			args.Append(" -f ").Append(sourceFile);
			args.Append(" -o ").Append(destFile);

			//args.Append(" -t ").Append("");
			fileSystem.CreateDir(GetPath(destFile));

			int result = fileSystem.SystemRun(exeFileName, { args });
			if (result != 0)
			{
				return nullptr;
			}

			file = cache.GetFile(cachedAsset, false);
		}

		if (file == nullptr)
		{
			file = cache.GetFile(filePath, false);
		}

		if (file == nullptr)
			return nullptr;

		// Make sure the pointer is non-null and is a Resource subclass
		SPtr<Resource> resource = DynamicCast<Resource>(GetContext()->CreateObject(type_));
		if (!resource)
		{
			UNIQUE_LOGERROR("Could not load unknown resource type " + String(type_));
			return SPtr<Resource>();
		}

		UNIQUE_LOGDEBUG("Loading resource " + filePath);
		resource->SetName(filePath);

		if (!resource->Load())
		{
			return SPtr<Resource>();
		}

		return resource;*/
	}


	SPtr<Resource> TextureImporter::LoadDDS(const String& path)
	{
		ResourceCache& cache = GetSubsystem<ResourceCache>();
		SPtr<File> file = cache.GetFile(path);
		if (!file)
		{
			return nullptr;
		}
		
		ByteArray data = file->ReadAll();
		SPtr<Texture> texture(new Texture());

		CreateDDSTextureFromMemoryEx(renderDevice,
			reinterpret_cast<const byte*>(data.data()),
			data.size(),
			0, // maxSize
			TexLoadInfo.Usage,
			TexLoadInfo.Name,
			TexLoadInfo.BindFlags,
			TexLoadInfo.CPUAccessFlags,
			0, // miscFlags
			TexLoadInfo.IsSRGB, // forceSRGB
			*texture);
		
		Diligent::ITexture* pTex = (*texture);
		texture->desc_ = pTex->GetDesc();
		texture->CreateTextureView();
		return texture;
	}

}
