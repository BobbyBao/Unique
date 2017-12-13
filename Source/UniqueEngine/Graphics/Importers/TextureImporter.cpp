#include "Precompiled.h"
#include "TextureImporter.h"
#include "../texture.h"


namespace Unique
{

	TextureImporter::TextureImporter() : ResourceImporter(Texture::GetTypeStatic())
	{
	}

	SPtr<Resource> TextureImporter::Import(const String& filePath)
	{/*
		if (filePath.EndsWith(".raw"))
		{
			return loadRaw(filePath);
		}

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
		return nullptr;
	}


	SPtr<Resource> TextureImporter::loadRaw(const String& path)
	{
		ResourceCache& cache = GetSubsystem<ResourceCache>();
		SPtr<File> file = cache.GetFile(path);
		if (!file)
		{
			return nullptr;
		}

		int texWidth = (int)sqrt(file->GetSize() / 2);
		int texHeight = texWidth;
		int size = texWidth * texHeight * sizeof(ushort);

		SharedArrayPtr<byte> data(new byte[size]);
		ushort* dst_mem = (ushort*)data.Get();
		if(size != file->Read(dst_mem, size))
		{
			return nullptr;
		}
		
		SPtr<Texture> texture(new Texture());

		return texture;
	}

}
