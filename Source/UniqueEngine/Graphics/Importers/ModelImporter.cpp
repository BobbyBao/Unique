#include "Precompiled.h"
#include "ModelImporter.h"
#include "../Geometry.h"
#include "../VertexBuffer.h"
#include "../Model.h"
#include "Animation/Animation.h"
#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"
#include <sstream>
#include "../Vertex.h"


namespace Unique
{
	ModelImporter::ModelImporter()
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

		String path = GetPath(file->GetGroupName() + filePath);
		ResourceDirGuard dirGuard(path, cache);
		if (filePath.EndsWith(".obj", false))
		{
			return LoadObjFile(*file);
		}

		SPtr<Model> model(new Model());
		model->Load(*file);
		return model;
	}

	#include <streambuf>

	class MemBuffer : public std::streambuf
	{
	public:
		MemBuffer(const char *begin, const char *end);
		MemBuffer(const char *str, size_t len);

	private:
		int_type underflow();
		int_type uflow();
		int_type pbackfail(int_type ch);
		std::streamsize showmanyc();

		// copy ctor and assignment not implemented;
		// copying not allowed
		MemBuffer(const MemBuffer &);
		MemBuffer &operator= (const MemBuffer &);

	private:
		const char * const begin_;
		const char * const end_;
		const char * current_;
	};
	
	MemBuffer::MemBuffer(const char *begin, const char *end) :
		begin_(begin),
		end_(end),
		current_(begin_)
	{
		assert(std::less_equal<const char *>()(begin_, end_));
	}

	MemBuffer::MemBuffer(const char *str, size_t len) :
		begin_(str),
		end_(begin_ + len),
		current_(begin_)
	{
	}

	MemBuffer::int_type MemBuffer::underflow()
	{
		if (current_ == end_)
			return traits_type::eof();

		return traits_type::to_int_type(*current_);
	}

	MemBuffer::int_type MemBuffer::uflow()
	{
		if (current_ == end_)
			return traits_type::eof();

		return traits_type::to_int_type(*current_++);
	}

	MemBuffer::int_type MemBuffer::pbackfail(int_type ch)
	{
		if (current_ == begin_ || (ch != traits_type::eof() && ch != current_[-1]))
			return traits_type::eof();

		return traits_type::to_int_type(*--current_);
	}

	std::streamsize MemBuffer::showmanyc()
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
		MemBuffer buf(&bytes[0], bytes.size());
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

	tinyobj::attrib_t attrib;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;

	SPtr<Resource> ModelImporter::LoadObjFile(IStream& source)
	{
		attrib.vertices.clear();
		attrib.normals.clear();
		attrib.texcoords.clear();
		attrib.colors.clear();
		shapes.clear();
		materials.clear();

		std::string err;
		ByteArray bytes = source.ReadAll();
		MemBuffer buf(&bytes[0], bytes.size());
		std::istream stream(&buf, true);
		MaterialStringStreamReader matReader(GetPath(source.GetName()));
		bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &err, &stream, &matReader);
		if(!ret)
		{
			UNIQUE_LOGERROR(err.c_str());
			return nullptr;
		}
		
		if(attrib.normals.size() > 0)
		{
			return LoadModel<VertexPosNormTex>();
		}
		else
		{
			return LoadModel<VertexPosTex>();
		}
	}

	void SetNormal(VertexPosNormTex& vertex, const index_t& index)
	{
		vertex.normal = 
		{
			attrib.normals[3 * index.normal_index + 0],
			attrib.normals[3 * index.normal_index + 1],
			attrib.normals[3 * index.normal_index + 2]
		};
	}
		
	void SetNormal(VertexPosTex& vertex, const index_t& index)
	{
	}

	SPtr<Material> CreateMaterial(material_t& mat)
	{
		SPtr<Material> pMat(new Material());
		pMat->SetShaderAttr(ResourceRef::Create<Shader>("Shaders/Default.shader"));
		if (!mat.diffuse_texname.empty())
		{
			pMat->SetTexture("DiffMap", ResourceRef::Create<Texture>(mat.diffuse_texname.c_str()));
		}
		return pMat;
	}

	template<class Vertex>
	SPtr<Resource> ModelImporter::LoadModel()
	{
		Vector<SPtr<VertexBuffer>> vertexBuffers;
		PODVector<unsigned> morphRangeStarts;
        PODVector<unsigned> morphRangeCounts;
		Vector<SPtr<IndexBuffer>> indexBuffers;

		SPtr<Model> model(new Model());
		model->SetNumGeometries(shapes.size());
		BoundingBox bundingBox;
	
		model->loadGeometries_.resize(shapes.size());

		Vector<Vertex> vertices;
		Vector<uint32_t> indices;
		HashMap<Vertex, uint32_t> uniqueVertices = {};
		uint indexOffset = 0;
        for (size_t i = 0; i < shapes.size(); i++)
		{	
			const auto& shape = shapes[i];
			uint indexCount = 0;
            for (const auto& index : shape.mesh.indices)
			{
                Vertex vertex = {};
                vertex.position =
				{
                    attrib.vertices[3 * index.vertex_index + 0],
                    attrib.vertices[3 * index.vertex_index + 1],
                    attrib.vertices[3 * index.vertex_index + 2]
                };

				bundingBox.Merge(vertex.position);
				
				SetNormal(vertex, index);

                vertex.texCoord =
				{
                    attrib.texcoords[2 * index.texcoord_index + 0],
                    1.0f - attrib.texcoords[2 * index.texcoord_index + 1]
                };

                //vertex.color = {1.0f, 1.0f, 1.0f};

                if (uniqueVertices.count(vertex) == 0)
				{
                    uniqueVertices[vertex] = static_cast<uint32_t>(vertices.size());
					vertices.push_back(vertex);
                }

                indices.push_back(uniqueVertices[vertex]); 
				indexCount++;
            }
					
			model->loadGeometries_[i].resize(1);

			GeometryDesc& desc = model->loadGeometries_[i][0];
			desc.type_ = PrimitiveTopology::PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
			desc.vbRef_ = vertexBuffers.size();
			desc.ibRef_ = indexBuffers.size();
			desc.indexStart_ = indexOffset;
			desc.indexCount_ = indexCount;
			desc.lodDistance_ = 0.0f;
			indexOffset += indexCount;
			SPtr<VertexBuffer> pVertexBuffer(new VertexBuffer());
			pVertexBuffer->Create(std::move(vertices));
			vertexBuffers.emplace_back(pVertexBuffer);
			indexBuffers.emplace_back(new IndexBuffer(std::move(indices)));

			int matId = shape.mesh.material_ids[0];
			if (matId >= materials.size())
			{
				UNIQUE_LOGERROR("Error material id : ", matId);
				break;
			}

			auto& mat = materials[matId];

			model->materials_.push_back(CreateMaterial(mat));
		}

		model->SetBoundingBox(bundingBox);
		model->SetVertexBuffers(vertexBuffers, morphRangeStarts, morphRangeCounts);
		model->SetIndexBuffers(indexBuffers);
		return model;
	}

	AnimationImporter::AnimationImporter()
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
