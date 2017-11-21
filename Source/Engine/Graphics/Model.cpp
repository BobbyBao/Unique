#include "Precompiled.h"
#include "Model.h"



namespace Unique
{
	Model::Model()
	{
	}


	Model::~Model()
	{
	}

	// Loads the vertices with position and normal from the specified Wavefront OBJ model file.
	Vector<VertexPositionNormal> Model::LoadObjModel(const String& filename)
	{
		// Read obj file
		std::ifstream file(filename.CString());
		if (!file.good())
			UNIQUE_LOGERROR("failed to load model from file: \"" + filename + "\"");

		Vector<Gs::Vector3f> coords, normals;
		Vector<VertexPositionNormal> vertices;

		while (!file.eof())
		{
			// Read each line
			std::string line;
			std::getline(file, line);

			std::stringstream s;
			s << line;

			// Parse line
			std::string mode;
			s >> mode;

			if (mode == "v")
			{
				Gs::Vector3f v;
				s >> v.x;
				s >> v.y;
				s >> v.z;
				coords.push_back(v);
			}
			else if (mode == "vn")
			{
				Gs::Vector3f n;
				s >> n.x;
				s >> n.y;
				s >> n.z;
				normals.push_back(n);
			}
			else if (mode == "f")
			{
				unsigned int v = 0, vn = 0;

				for (int i = 0; i < 3; ++i)
				{
					s >> v;
					s.ignore(2);
					s >> vn;
					vertices.push_back({ coords[v - 1], normals[vn - 1] });
				}
			}
		}

		return vertices;
	}

}