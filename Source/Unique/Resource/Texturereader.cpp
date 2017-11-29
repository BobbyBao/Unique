#include "Precompiled.h"
#include "texturereader.h"
#include "../graphics/texture.h"


namespace Unique
{

	TextureReader::TextureReader() : ResourceReader(Texture::GetTypeStatic())
	{
	}

	SPtr<Resource> TextureReader::load(const String& filePath)
	{
		if (filePath.EndsWith(".raw"))
		{
			return loadRaw(filePath);
		}

		String cachedAsset = ReplaceExtension(filePath, ".ktx");
		ResourceCache& cache = Subsystem<ResourceCache>();
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

		if (!resource->Load(*(file.Get())))
		{
			return SPtr<Resource>();
		}

		return resource;
	}


	SPtr<Resource> TextureReader::loadRaw(const String& path)
	{
		ResourceCache& cache = Subsystem<ResourceCache>();
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

		for (int i = 0; i < texWidth * texHeight; ++i)
		{
			dst_mem[i] = /*bx::halfFromFloat*/(dst_mem[i]/* / 65535.0f*/);
		}

		SPtr<Texture> texture(new Texture());
		/*
		const bgfx::Memory* mem = bgfx::makeRef(data.Get(), size);

		texture->create2D((uint16_t)texWidth, (uint16_t)texHeight,
			bgfx::TextureFormat::R16, data, size);
		*/
		return texture;
	}

}
