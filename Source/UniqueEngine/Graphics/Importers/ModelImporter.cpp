#include "Precompiled.h"
#include "ModelImporter.h"
#include "../Geometry.h"
#include "../VertexBuffer.h"
#include "../Model.h"
#include "Animation/Animation.h"
#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"
#include <sstream>

namespace Unique
{
	struct Vertex 
	{
		Vector3 pos;
		Vector3 normal;
		//Vector3 color;
		Vector2 texCoord;
		/*
		static VkVertexInputBindingDescription getBindingDescription() {
			VkVertexInputBindingDescription bindingDescription = {};
			bindingDescription.binding = 0;
			bindingDescription.stride = sizeof(Vertex);
			bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

			return bindingDescription;
		}

		static std::array<VkVertexInputAttributeDescription, 3> getAttributeDescriptions() {
			std::array<VkVertexInputAttributeDescription, 3> attributeDescriptions = {};

			attributeDescriptions[0].binding = 0;
			attributeDescriptions[0].location = 0;
			attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
			attributeDescriptions[0].offset = offsetof(Vertex, pos);

			attributeDescriptions[1].binding = 0;
			attributeDescriptions[1].location = 1;
			attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
			attributeDescriptions[1].offset = offsetof(Vertex, color);

			attributeDescriptions[2].binding = 0;
			attributeDescriptions[2].location = 2;
			attributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
			attributeDescriptions[2].offset = offsetof(Vertex, texCoord);

			return attributeDescriptions;
		}
		*/
		bool operator==(const Vertex& other) const {
			return pos == other.pos && normal == other.normal && texCoord == other.texCoord;
		}
	};
}

namespace std 
{
	using namespace Unique;
	
    template<> struct hash<Vertex>
	{
        size_t operator()(Vertex const& vertex) const
		{		
			size_t seed = 0;
			hash<Vector3> hashVec3;
			hash<Vector2> hashVec2;
			HashCombine(seed, hashVec3(vertex.pos));
			HashCombine(seed, hashVec3(vertex.normal));
			HashCombine(seed, hashVec2(vertex.texCoord));
			return seed;
		}
    };
}

namespace Unique
{
	ModelImporter::ModelImporter()
		: ResourceImporter(Model::GetTypeStatic())
	{
	}

	ModelImporter::~ModelImporter()
	{
	}

	SPtr<Resource> ModelImporter::Import(const String& filePath)
	{	
		SPtr<File> file = cache.GetFile(filePath);
		if (!file)
		{
			return nullptr;
		}
	
		if (filePath.EndsWith(".obj", false))
		{
			return LoadObjFile(*file);
		}

		SPtr<Model> model(new Model());
		model->Load(*file);
		return model;
	}

	#include <streambuf>

	class char_array_buffer : public std::streambuf
	{
	public:
		char_array_buffer(const char *begin, const char *end);
		char_array_buffer(const char *str, size_t len);

	private:
		int_type underflow();
		int_type uflow();
		int_type pbackfail(int_type ch);
		std::streamsize showmanyc();

		// copy ctor and assignment not implemented;
		// copying not allowed
		char_array_buffer(const char_array_buffer &);
		char_array_buffer &operator= (const char_array_buffer &);

	private:
		const char * const begin_;
		const char * const end_;
		const char * current_;
	};
	
	char_array_buffer::char_array_buffer(const char *begin, const char *end) :
		begin_(begin),
		end_(end),
		current_(begin_)
	{
		assert(std::less_equal<const char *>()(begin_, end_));
	}

	char_array_buffer::char_array_buffer(const char *str, size_t len) :
		begin_(str),
		end_(begin_ + len),
		current_(begin_)
	{
	}

	char_array_buffer::int_type char_array_buffer::underflow()
	{
		if (current_ == end_)
			return traits_type::eof();

		return traits_type::to_int_type(*current_);
	}

	char_array_buffer::int_type char_array_buffer::uflow()
	{
		if (current_ == end_)
			return traits_type::eof();

		return traits_type::to_int_type(*current_++);
	}

	char_array_buffer::int_type char_array_buffer::pbackfail(int_type ch)
	{
		if (current_ == begin_ || (ch != traits_type::eof() && ch != current_[-1]))
			return traits_type::eof();

		return traits_type::to_int_type(*--current_);
	}

	std::streamsize char_array_buffer::showmanyc()
	{
		assert(std::less_equal<const char *>()(current_, end_));
		return end_ - current_;
	}

	using namespace tinyobj;

	class MaterialStringStreamReader : public MaterialReader 
	{
	public:
	MaterialStringStreamReader(const String& path) : modelPath(path)
	{
	}

	virtual ~MaterialStringStreamReader() {}

	virtual bool operator()(const std::string& matId,
							std::vector<material_t>* materials,
							std::map<std::string, int>* matMap,
							std::string* err)
	{
		auto& cache = GetSubsystem<ResourceCache>();
		
		SPtr<File> file = cache.GetFile(modelPath + matId.c_str());
		ByteArray bytes = file->ReadAll();
		char_array_buffer buf(&bytes[0], bytes.size());
		std::istream stream(&buf, true);

		std::string warning;
		LoadMtl(matMap, materials, &stream, &warning);

		if (!warning.empty()) {
		if (err) {
			(*err) += warning;
		}
		}
		return true;
	}

	private:
		String modelPath;
	};

	SPtr<Resource> ModelImporter::LoadObjFile(IStream& source)
	{
		tinyobj::attrib_t attrib;
		std::vector<tinyobj::shape_t> shapes;
		std::vector<tinyobj::material_t> materials;

		std::string err;
		ByteArray bytes = source.ReadAll();
		char_array_buffer buf(&bytes[0], bytes.size());
		std::istream stream(&buf, true);
		MaterialStringStreamReader matReader(GetPath(source.GetName()));
		bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &err, &stream, &matReader);
		if(!ret)
		{
			UNIQUE_LOGERROR(err.c_str());
			return nullptr;
		}
		
		Vector<SPtr<VertexBuffer>> vertexBuffers;
		PODVector<unsigned> morphRangeStarts;
        PODVector<unsigned> morphRangeCounts;
		Vector<SPtr<IndexBuffer>> indexBuffers;

		SPtr<Model> model(new Model());
		model->SetNumGeometries(shapes.size());
		BoundingBox bundingBox;
	
		model->loadGeometries_.resize(shapes.size());

        for (size_t i = 0; i < shapes.size(); i++)
		{	
			const auto& shape  = shapes[i];
			Vector<Vertex> vertices;
			Vector<uint32_t> indices;
			HashMap<Vertex, uint32_t> uniqueVertices = {};
            for (const auto& index : shape.mesh.indices)
			{
                Vertex vertex = {};
                vertex.pos = 
				{
                    attrib.vertices[3 * index.vertex_index + 0],
                    attrib.vertices[3 * index.vertex_index + 1],
                    attrib.vertices[3 * index.vertex_index + 2]
                };

				bundingBox.Merge(vertex.pos);
				
                vertex.normal = 
				{
                    attrib.normals[3 * index.normal_index + 0],
                    attrib.normals[3 * index.normal_index + 1],
                    attrib.normals[3 * index.normal_index + 2]
                };

                vertex.texCoord =
				{
                    attrib.texcoords[2 * index.texcoord_index + 0],
                    1.0f - attrib.texcoords[2 * index.texcoord_index + 1]
                };

                //vertex.color = {1.0f, 1.0f, 1.0f};

#ifdef NO_HASH
                indices.push_back(vertices.size()); 
				vertices.push_back(vertex);
#else
                if (uniqueVertices.count(vertex) == 0)
				{
                    uniqueVertices[vertex] = static_cast<uint32_t>(vertices.size());
					vertices.push_back(vertex);
                }

                indices.push_back(uniqueVertices[vertex]); 
#endif
				
            }
					
			model->loadGeometries_[i].resize(1);
			GeometryDesc& desc = model->loadGeometries_[i][0];

			desc.type_ = PrimitiveTopology::TRIANGLE_LIST;
			desc.vbRef_ = vertexBuffers.size();
			desc.ibRef_ = indexBuffers.size();
			desc.indexStart_ = 0;
			desc.indexCount_ = indices.size();
			desc.lodDistance_ = 0.0f;

			SPtr<VertexBuffer> pVertexBuffer(new VertexBuffer());
			pVertexBuffer->Create(std::move(vertices));
			vertexBuffers.emplace_back(pVertexBuffer);
			indexBuffers.emplace_back(new IndexBuffer(std::move(indices)));

		}

		model->SetBoundingBox(bundingBox);
		model->SetVertexBuffers(vertexBuffers, morphRangeStarts, morphRangeCounts);
		model->SetIndexBuffers(indexBuffers);
		return model;
	}

	AnimationImporter::AnimationImporter() : ResourceImporter(Animation::GetTypeStatic())
	{
	}

	AnimationImporter::~AnimationImporter()
	{
	}

	SPtr<Resource> AnimationImporter::Import(const String& path)
	{
		SPtr<File> file = cache.GetFile(path);
		if (!file)
		{
			return nullptr;
		}

		SPtr<Animation> anim(new Animation());
		anim->Load(*file);
		return anim;
	}
}
