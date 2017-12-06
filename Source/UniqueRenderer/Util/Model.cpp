
/* * * * * * * * * * * * * Author's note * * * * * * * * * * * *\
*   _       _   _       _   _       _   _       _     _ _ _ _   *
*  |_|     |_| |_|     |_| |_|_   _|_| |_|     |_|  _|_|_|_|_|  *
*  |_|_ _ _|_| |_|     |_| |_|_|_|_|_| |_|     |_| |_|_ _ _     *
*  |_|_|_|_|_| |_|     |_| |_| |_| |_| |_|     |_|   |_|_|_|_   *
*  |_|     |_| |_|_ _ _|_| |_|     |_| |_|_ _ _|_|  _ _ _ _|_|  *
*  |_|     |_|   |_|_|_|   |_|     |_|   |_|_|_|   |_|_|_|_|    *
*                                                               *
*                     http://www.humus.name                     *
*                                                                *
* This file is a part of the work done by Humus. You are free to   *
* use the code in any way you like, modified, unmodified or copied   *
* into your own work. However, I expect you to respect these points:  *
*  - If you use this file and its contents unmodified, or use a major *
*    part of this file, please credit the author and leave this note. *
*  - For use in anything commercial, please request my approval.     *
*  - Share your work and ideas too as much as you can.             *
*                                                                *
\* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include "Model.h"
#include "Tokenizer.h"

#include "Hash.h"
#include "Math/MathDefs.h"
#include "Util/Array.h"

using namespace Unique;


Model::Model(){
	vertexFormat = VF_NONE;
	vertexBuffer = VB_NONE;
	indexBuffer  = IB_NONE;
	nIndices = 0;

	lastVertexCount = 0;
	lastVertices = NULL;
	lastIndices = NULL;
	lastFormat = NULL;
}

Model::~Model(){
	clear();
}

void subDivide(float3 *&dest, const float3 &v0, const float3 &v1, const float3 &v2, int level){
	if (level){
		level--;
		float3 v3 = normalize(v0 + v1);
		float3 v4 = normalize(v1 + v2);
		float3 v5 = normalize(v2 + v0);

		subDivide(dest, v0, v3, v5, level);
		subDivide(dest, v3, v4, v5, level);
		subDivide(dest, v3, v1, v4, level);
		subDivide(dest, v5, v4, v2, level);
	} else {
		*dest++ = v0;
		*dest++ = v1;
		*dest++ = v2;
	}
}

void Model::createSphere(const int subDivLevel){
	const int nVertices = 8 * 3 * (1 << (2 * subDivLevel));

	float3 *vertices = new float3[nVertices];

	// Tessellate a octahedron
	float3 px0(-1,  0,  0);
	float3 px1( 1,  0,  0);
	float3 py0( 0, -1,  0);
	float3 py1( 0,  1,  0);
	float3 pz0( 0,  0, -1);
	float3 pz1( 0,  0,  1);

	float3 *dest = vertices;
	subDivide(dest, py0, px0, pz0, subDivLevel);
	subDivide(dest, py0, pz0, px1, subDivLevel);
	subDivide(dest, py0, px1, pz1, subDivLevel);
	subDivide(dest, py0, pz1, px0, subDivLevel);
	subDivide(dest, py1, pz0, px0, subDivLevel);
	subDivide(dest, py1, px0, pz1, subDivLevel);
	subDivide(dest, py1, pz1, px1, subDivLevel);
	subDivide(dest, py1, px1, pz0, subDivLevel);

	ASSERT(dest - vertices == nVertices);

	addStream(TYPE_VERTEX, 3, nVertices, (float *) vertices, NULL, false);
	nIndices = nVertices;
	addBatch(0, nVertices);
}

StreamID Model::findStream(const AttributeType type, const uint index) const {
	uint count = 0;
	for (uint i = 0; i < streams.size(); i++){
		if (streams[i].type == type){
			if (count == index) return i;
			count++;
		}
	}
	return -1;
}

void Model::changeAllGeneric(const bool excludeVertex){
	for (uint i = 0; i < streams.size(); i++){
		if (!excludeVertex || streams[i].type != TYPE_VERTEX){
			streams[i].type = TYPE_GENERIC;
		}
	}
}

BatchID Model::addBatch(const uint startIndex, const uint nIndices){
	Batch batch;

	batch.startIndex = startIndex;
	batch.nIndices = nIndices;
//	batch.startVertex = startVertex;
//	batch.nVertices = nVertices;
	batch.startVertex = 0;
	batch.nVertices = 0;
	batches.push_back(batch);
	return batches.size() - 1;
}

bool Model::mergeBatches(const BatchID batch, const BatchID toMerge){
	if (batches[batch].startIndex + batches[batch].nIndices != batches[toMerge].startIndex) return false;

	batches[batch].nIndices += batches[toMerge].nIndices;
	batches.erase(batches.begin() + toMerge);

	return true;
}

void Model::getBoundingBox(const StreamID stream, float *minCoord, float *maxCoord) const {
	float *src = streams[stream].vertices;
	uint nVertices = streams[stream].nVertices;
	uint nComp = streams[stream].nComponents;

	for (uint j = 0; j < nComp; j++){
		minCoord[j] = maxCoord[j] = src[j];
	}
	src += nComp;
	for (uint i = 1; i < nVertices; i++){
		for (uint j = 0; j < nComp; j++){
			if (*src < minCoord[j]) minCoord[j] = *src;
			if (*src > maxCoord[j]) maxCoord[j] = *src;
			src++;
		}		
	}
}

int readSignedInt(Tokenizer &tok){
	char *str = tok.next();
	if (str[0] == '-'){
		str = tok.next();
		return -atoi(str);
	} else {
		if (str[0] == '+'){
			str = tok.next();
		}
		return atoi(str);
	}
}

float readFloat(Tokenizer &tok){
	char *str = tok.next();
	if (str[0] == '-'){
		str = tok.next();
		return -(float) atof(str);
	} else {
		if (str[0] == '+'){
			str = tok.next();
		}
		return (float) atof(str);
	}
}

struct ObjMaterial {
	char *name;
	Vector <uint> *indices;
};

uint getObjMaterial(Vector <ObjMaterial> &materials, const char *str){
	for (uint i = 0; i < materials.size(); i++){
		if (strcmp(str, materials[i].name) == 0){
			return i;
		}
	}
	ObjMaterial mat;
	mat.name = new char[strlen(str) + 1];
	mat.indices = new Vector <uint>();
	strcpy(mat.name, str);
	materials.push_back(mat);
	return materials.size() - 1;
}

uint *rearrange(Vector <ObjMaterial> &materials, const uint *srcIndices, const uint nIndices){
	uint *indices = new uint[nIndices];

	uint currIndex = 0;
	for (uint i = 0; i < materials.size(); i++){
		Vector <uint> *mInds = materials[i].indices;
		for (uint j = 0; j < mInds->size(); j += 2){
			memcpy(indices + currIndex, srcIndices + (*mInds)[j], (*mInds)[j + 1] * sizeof(uint));
			currIndex += (*mInds)[j + 1];
		}
	}

	ASSERT(currIndex == nIndices);

	return indices;
}

bool Model::loadObj(const char *fileName){
	Tokenizer tok, lineTok;
	if (!tok.setFile(fileName)){
		char str[256];
		sprintf(str, "Couldn't open \"%s\"", fileName);
		ErrorMsg(str);
		return false;
	}

	Array <vec3> vertices;
	Array <vec3> normals;
	Array <vec2> texCoords;
	Array <vec3> tangents;
	Array <vec3> binormals;

	Array <uint> vtxIndices;
	Array <uint> nrmIndices;
	Array <uint> txcIndices;

	Vector <ObjMaterial> materials;

	ObjMaterial mat;
	mat.name = new char[2];
	strcpy(mat.name, " ");
	mat.indices = new Vector <uint>();
	materials.push_back(mat);
	uint currIndex = 0;
	uint currMaterial = 0;


	uint n, vStart, nStart, tStart;
	char *str;

	while ((str = tok.next()) != NULL){
		switch (str[0]){
			case 'f':
				lineTok.setString(tok.nextLine());

				vStart = vtxIndices.size();
				nStart = nrmIndices.size();
				tStart = txcIndices.size();
				n = 0;
				while ((str = lineTok.next()) != NULL){
					if (n > 2){
						vtxIndices.push_back(vtxIndices[vStart]);
						vtxIndices.push_back(vtxIndices[vtxIndices.size() - 2]);
						if (texCoords.size()){
							txcIndices.push_back(txcIndices[tStart]);
							txcIndices.push_back(txcIndices[txcIndices.size() - 2]);
						}
						if (normals.size()){
							nrmIndices.push_back(nrmIndices[nStart]);
							nrmIndices.push_back(nrmIndices[nrmIndices.size() - 2]);
						}
					}

					int index;
					if (str[0] == '-')
						index = vertices.size() - atoi(lineTok.next());
					else
						index = atoi(str) - 1;
					vtxIndices.push_back(index);

					if (texCoords.size() || normals.size()) lineTok.goToNext();
					if (texCoords.size()){
						str = lineTok.next();
						if (str[0] == '-')
							index = texCoords.size() - atoi(lineTok.next());
						else
							index = atoi(str) - 1;
						txcIndices.push_back(index);
					}
					if (normals.size()){
						lineTok.goToNext();

						str = lineTok.next();
						if (str[0] == '-')
							index = normals.size() - atoi(lineTok.next());
						else
							index = atoi(str) - 1;
						nrmIndices.push_back(index);
					}

					n++;
				}
				break;
			case 'v':
				float x, y, z;
				switch (str[1]){
					case '\0':
						x = readFloat(tok);
						y = readFloat(tok);
						z = readFloat(tok);
						vertices.push_back(vec3(x, y, z));
						break;
					case 'n':
						x = readFloat(tok);
						y = readFloat(tok);
						z = readFloat(tok);
						normals.push_back(vec3(x, y, z));
						break;
					case 't':
						x = readFloat(tok);
						y = readFloat(tok);
						texCoords.push_back(vec2(x, y));
						break;
				}
				break;
			case '#':
				if (*tok.next() == '_'){
					tok.goToNext();
					str = tok.next();
					if (stricmp(str, "tangent") == 0){
						x = readFloat(tok);
						y = readFloat(tok);
						z = readFloat(tok);
						tangents.push_back(vec3(x, y, z));
					} else if (stricmp(str, "binormal") == 0){
						x = readFloat(tok);
						y = readFloat(tok);
						z = readFloat(tok);
						binormals.push_back(vec3(x, y, z));
					}
				} else {
					tok.goToNextLine();
				}
				break;
			case 'u':
				if (vtxIndices.size()){
					materials[currMaterial].indices->push_back(currIndex);
					materials[currMaterial].indices->push_back(vtxIndices.size() - currIndex);
					currIndex = vtxIndices.size();
				} else {
					// Ensure we don't get an empty material set in the beginning
					delete materials[0].name;
					delete materials[0].indices;
					materials.clear();
				}

				currMaterial = getObjMaterial(materials, tok.next());
				break;
			default:
				tok.goToNextLine();
				break;
		}
	}

	materials[currMaterial].indices->push_back(currIndex);
	materials[currMaterial].indices->push_back(vtxIndices.size() - currIndex);

	nIndices = vtxIndices.size();

	addStream(TYPE_VERTEX, 3, vertices.size(), (float *) vertices.abandonArray(), rearrange(materials, vtxIndices.data(), nIndices), false);
	if (texCoords.size()){
		ASSERT(txcIndices.size() == nIndices);

		uint *indices = rearrange(materials, txcIndices.data(), nIndices);

		addStream(TYPE_TEXCOORD, 2, texCoords.size(), (float *) texCoords.abandonArray(), indices, false);
		if (tangents.size()){
			uint *tIndices = new uint[nIndices];
			memcpy(tIndices, indices, nIndices * sizeof(uint));

			addStream(TYPE_TEXCOORD, 3, tangents.size(), (float *) tangents.abandonArray(), tIndices, false);
		}
		if (binormals.size()){
			uint *bIndices = new uint[nIndices];
			memcpy(bIndices, indices, nIndices * sizeof(uint));

			addStream(TYPE_TEXCOORD, 3, binormals.size(), (float *) binormals.abandonArray(), bIndices, false);
		}
	}
	if (normals.size()){
		ASSERT(nrmIndices.size() == nIndices);

		addStream(TYPE_NORMAL, 3, normals.size(), (float *) normals.abandonArray(), rearrange(materials, nrmIndices.data(), nIndices), false);
	}

	// Fix batches
	currIndex = 0;
	uint startIndex = 0;
	for (uint i = 0; i < materials.size(); i++){
		Vector <uint> *mInds = materials[i].indices;
		for (uint j = 0; j < mInds->size(); j += 2){
			currIndex += (*mInds)[j + 1];
		}
		addBatch(startIndex, currIndex - startIndex);
		startIndex = currIndex;

		delete materials[i].indices;
		delete materials[i].name;
	}

	return true;
}

bool textureNameAlphabetical(const char ch){
	return ((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') || ch == '_' || ch == '.' || ch == '-');
}

uint Model::getVertexSize() const {
	return getComponentCount() * sizeof(float);
}

uint Model::getComponentCount() const {
	uint count = 0;
	for (uint i = 0; i < streams.size(); i++){
		count += streams[i].nComponents;
	}
	return count;
}

uint Model::getComponentCount(const StreamID *cStreams, const uint nStreams) const {
	uint count = 0;
	for (uint i = 0; i < nStreams; i++){
		count += streams[cStreams[i]].nComponents;
	}
	return count;
}

StreamID Model::addStream(const AttributeType type, const int nComponents, const uint nVertices, float *vertices, uint *indices, bool optimized){
	if (indices == NULL) indices = getArrayIndices(nVertices);

	Stream stream;
	stream.nVertices = nVertices;
	stream.vertices  = vertices;
	stream.indices   = indices;
	stream.type      = type;
	stream.nComponents = nComponents;
	stream.optimized = optimized;
	streams.push_back(stream);
	return streams.size() - 1;
}

void Model::removeStream(const StreamID stream){
	if (stream >= 0){
		delete streams[stream].vertices;
		delete streams[stream].indices;

		streams.erase(streams.begin() + stream);
	}
}

void Model::scale(const StreamID stream, const float *scaleFactors){
	uint nComp = streams[stream].nComponents;

	float *dest = streams[stream].vertices;
	for (uint i = 0; i < streams[stream].nVertices; i++){
		for (uint j = 0; j < nComp; j++){
			*dest++ *= scaleFactors[j];
		}
	}
}

void Model::translate(const StreamID stream, const float *translation){
	uint nComp = streams[stream].nComponents;

	float *dest = streams[stream].vertices;
	for (uint i = 0; i < streams[stream].nVertices; i++){
		for (uint j = 0; j < nComp; j++){
			*dest++ += translation[j];
		}
	}
}

void Model::flipComponents(const StreamID stream, const uint c0, const uint c1){
	for (uint i = 0; i < streams[stream].nVertices; i++){
		float *src = streams[stream].vertices + i * streams[stream].nComponents;

		float temp = src[c0];
		src[c0] = src[c1];
		src[c1] = temp;
	}
}

void Model::reverseWinding(){
	for (uint j = 0; j < streams.size(); j++){
		uint *indices = streams[j].indices;
		for (uint i = 0; i < nIndices; i += 3){
			uint temp = indices[i];
			indices[i] = indices[i + 2];
			indices[i + 2] = temp;
		}
	}
}

bool Model::flipNormals(){
	StreamID normalStream = findStream(TYPE_NORMAL);
	if (normalStream < 0) return false;

	vec3 *normals = (vec3 *) streams[normalStream].vertices;
	uint nVertices = streams[normalStream].nVertices;

	for (uint i = 0; i < nVertices; i++){
		normals[i] = -normals[i];
	}

	return true;
}

bool Model::computeNormals(const bool flat){
	StreamID vertexStream = findStream(TYPE_VERTEX);
	if (vertexStream < 0) return false;

	optimizeStream(vertexStream);

	vec3 *vertices = (vec3 *) streams[vertexStream].vertices;
	uint *indices = streams[vertexStream].indices;

	if (flat){
		uint nFaces = nIndices / 3;

		vec3 *normals = new vec3[nFaces];
		uint *nrmIndices = new uint[nIndices];
		for (uint i = 0; i < nFaces; i++){
			vec3 v0 = vertices[indices[3 * i    ]];
			vec3 v1 = vertices[indices[3 * i + 1]];
			vec3 v2 = vertices[indices[3 * i + 2]];

			normals[i] = normalize(cross(v1 - v0, v2 - v0));
			nrmIndices[3 * i    ] = i;
			nrmIndices[3 * i + 1] = i;
			nrmIndices[3 * i + 2] = i;
		}
		addStream(TYPE_NORMAL, 3, nFaces, (float *) normals, nrmIndices, false);

	} else {
		uint nVertices = streams[vertexStream].nVertices;

		vec3 *normals = new vec3[nVertices];
		memset(normals, 0, nVertices * sizeof(vec3));

		for (uint i = 0; i < nIndices; i += 3){
			vec3 v0 = vertices[indices[i    ]];
			vec3 v1 = vertices[indices[i + 1]];
			vec3 v2 = vertices[indices[i + 2]];

			vec3 normal = normalize(cross(v1 - v0, v2 - v0));

			normals[indices[i    ]] += normal;
			normals[indices[i + 1]] += normal;
			normals[indices[i + 2]] += normal;
		}

		for (uint j = 0; j < nVertices; j++){
			normals[j] = normalize(normals[j]);
		}

		uint *nrmIndices = new uint[nIndices];
		memcpy(nrmIndices, indices, nIndices * sizeof(uint));

		addStream(TYPE_NORMAL, 3, nVertices, (float *) normals, nrmIndices, false);
	}

	return true;
}

void tangentVectors(const vec3 &v0, const vec3 &v1, const vec3 &v2, const vec2 &t0, const vec2 &t1, const vec2 &t2, vec3 &sdir, vec3 &tdir, vec3 &normal){
	vec3 dv0 = v1 - v0;
	vec3 dv1 = v2 - v0;

	vec2 dt0 = t1 - t0;
	vec2 dt1 = t2 - t0;

	float r = 1.0f / (dt0.x * dt1.y - dt1.x * dt0.y);
	sdir = vec3(dt1.y * dv0.x - dt0.y * dv1.x, dt1.y * dv0.y - dt0.y * dv1.y, dt1.y * dv0.z - dt0.y * dv1.z) * r;
	tdir = vec3(dt0.x * dv1.x - dt1.x * dv0.x, dt0.x * dv1.y - dt1.x * dv0.y, dt0.x * dv1.z - dt1.x * dv0.z) * r;
	normal = normalize(cross(dv0, dv1));
}

bool Model::computeTangentSpace(const bool flat){
	StreamID streams[2] = { findStream(TYPE_VERTEX), findStream(TYPE_TEXCOORD) };

	if (streams[0] < 0 || streams[1] < 0) return false;

	float *vertexArrays[2];
	uint *indices;

	uint nVertices = assemble(streams, 2, vertexArrays, &indices, true);

	vec3 *vertices  = (vec3 *) vertexArrays[0];
	vec2 *texCoords = (vec2 *) vertexArrays[1];

	if (flat){
		uint nFaces = nIndices / 3;

		vec3 *tangents  = new vec3[nFaces];
		vec3 *binormals = new vec3[nFaces];
		vec3 *normals   = new vec3[nFaces];

		uint *indicesS = new uint[nIndices];
		uint *indicesT = new uint[nIndices];
		uint *indicesN = new uint[nIndices];

		for (uint i = 0; i < nFaces; i++){
			vec3 v0 = vertices[indices[3 * i    ]];
			vec3 v1 = vertices[indices[3 * i + 1]];
			vec3 v2 = vertices[indices[3 * i + 2]];

			vec2 t0 = texCoords[indices[3 * i    ]];
			vec2 t1 = texCoords[indices[3 * i + 1]];
			vec2 t2 = texCoords[indices[3 * i + 2]];

			vec3 sdir, tdir, normal;
			tangentVectors(v0, v1, v2, t0, t1, t2, sdir, tdir, normal);

			sdir = normalize(sdir);
			tdir = normalize(tdir);

			tangents [i] = normalize(sdir);
			binormals[i] = normalize(tdir);
			normals  [i] = normal;

			indicesS[3 * i] = indicesS[3 * i + 1] = indicesS[3 * i + 2] = i;
			indicesT[3 * i] = indicesT[3 * i + 1] = indicesT[3 * i + 2] = i;
			indicesN[3 * i] = indicesN[3 * i + 1] = indicesN[3 * i + 2] = i;
		}

		addStream(TYPE_TANGENT,  3, nFaces, (float *) tangents,  indicesS, false);
		addStream(TYPE_BINORMAL, 3, nFaces, (float *) binormals, indicesT, false);
		addStream(TYPE_NORMAL,   3, nFaces, (float *) normals,   indicesN, false);

		delete [] indices;

	} else {
		vec3 *tangents  = new vec3[nVertices];
		vec3 *binormals = new vec3[nVertices];
		vec3 *normals   = new vec3[nVertices];

		memset(tangents,  0, nVertices * sizeof(vec3));
		memset(binormals, 0, nVertices * sizeof(vec3));
		memset(normals,   0, nVertices * sizeof(vec3));

		for (uint i = 0; i < nIndices; i += 3){
			vec3 v0 = vertices[indices[i    ]];
			vec3 v1 = vertices[indices[i + 1]];
			vec3 v2 = vertices[indices[i + 2]];

			vec2 t0 = texCoords[indices[i    ]];
			vec2 t1 = texCoords[indices[i + 1]];
			vec2 t2 = texCoords[indices[i + 2]];

			vec3 sdir, tdir, normal;
			tangentVectors(v0, v1, v2, t0, t1, t2, sdir, tdir, normal);

			sdir = normalize(sdir);
			tdir = normalize(tdir);

			tangents[indices[i    ]] += sdir;
			tangents[indices[i + 1]] += sdir;
			tangents[indices[i + 2]] += sdir;

			binormals[indices[i    ]] += tdir;
			binormals[indices[i + 1]] += tdir;
			binormals[indices[i + 2]] += tdir;

			normals[indices[i    ]] += normal;
			normals[indices[i + 1]] += normal;
			normals[indices[i + 2]] += normal;
		}
		
		for (uint j = 0; j < nVertices; j++){
			tangents [j] = normalize(tangents [j]);
			binormals[j] = normalize(binormals[j]);
			normals  [j] = normalize(normals  [j]);
		}

		uint *indicesS = new uint[nIndices];
		uint *indicesT = new uint[nIndices];
		memcpy(indicesS, indices, nIndices * sizeof(uint));
		memcpy(indicesT, indices, nIndices * sizeof(uint));

		addStream(TYPE_TANGENT,  3, nVertices, (float *) tangents,  indicesS, false);
		addStream(TYPE_BINORMAL, 3, nVertices, (float *) binormals, indicesT, false);
		addStream(TYPE_NORMAL,   3, nVertices, (float *) normals,   indices,  false);
	}

	delete [] vertices;
	delete [] texCoords;

	return true;
}

struct Edge {
	Edge(const uint i0, const uint i1){
		index[0] = min(i0, i1);
		index[1] = max(i0, i1);
	}

	uint index[2];
};

bool Model::addStencilVolume(){
	if (nIndices == 0) return true;

	StreamID vertexStream = findStream(TYPE_VERTEX);
	if (vertexStream < 0) return false;

	// Make sure the vertex position stream is properly and uniquely indexed
	optimizeStream(vertexStream);


	Hash hash(2, nIndices >> 3, nIndices);
	Vector <uint> list;
	Vector <vec3> normList;

	uint *newIndices = new uint[nIndices * 6];
	uint *dest = newIndices;

	vec3 *vertices = (vec3 *) streams[vertexStream].vertices;
	uint *indices = streams[vertexStream].indices;
	for (uint i = 0; i < nIndices; i += 3){
		vec3 v0 = vertices[indices[i]];
		vec3 v1 = vertices[indices[i + 1]];
		vec3 v2 = vertices[indices[i + 2]];
		vec3 normal = normalize(cross(v1 - v0, v2 - v0));

		uint prev = 2;
		for (uint k = 0; k < 3; k++){
			Edge edge(indices[i + prev], indices[i + k]);

			uint index;
			// Have we seen this edge before ...
			if (hash.insert(edge.index, &index)){
				// .. then create a quad between the two edges

				// Don't include edges that aren't at an angle (like the internal edge in a quad)
				if (Abs(dot(normal, normList[index]) - 1) > 0.01f){
					*dest++ = i + prev;
					*dest++ = list[2 * index];
					*dest++ = i + k;

					*dest++ = i + prev;
					*dest++ = list[2 * index + 1];
					*dest++ = list[2 * index];
				}
			} else {
				// .. otherwise store it for now
				list.push_back(i + prev);
				list.push_back(i + k);
				normList.push_back(normal);
			}
			prev = k;
		}
	}

	uint nNewIndices = (uint) (dest - newIndices);
	
	for (uint i = 0; i < streams.size(); i++){
		streams[i].indices = (uint *) realloc(streams[i].indices, (nIndices + nNewIndices) * sizeof(uint));
		for (uint k = 0; k < nNewIndices; k++){
			streams[i].indices[nIndices + k] = streams[i].indices[newIndices[k]];
		}
	}

	addBatch(nIndices, nNewIndices);

	delete newIndices;
	nIndices += nNewIndices;


	return true;
}


struct Triangle {
	vec3 pos[3];
	float *attributes[3];
	int batch;
};

int compareTriangles(const Triangle &elem0, const Triangle &elem1){
	return elem0.batch - elem1.batch;
}

void Model::cleanUp(){
	StreamID vertexStream = findStream(TYPE_VERTEX);
	if (vertexStream < 0) return;

	optimizeStream(vertexStream);

	vec3 *vertices = (vec3 *) streams[vertexStream].vertices;
	uint *indices   = streams[vertexStream].indices;
	uint nVertices  = streams[vertexStream].nVertices;

	Vector <Triangle> triangles;

	uint nAttrib = getComponentCount();

	for (uint n = 0; n < batches.size(); n++){
		uint endIndex = batches[n].startIndex + batches[n].nIndices;
		for (uint i = batches[n].startIndex; i < endIndex; i += 3){
			Triangle tri;
			tri.batch = n;

			for (uint j = 0; j < 3; j++){
				tri.pos[j] = vertices[indices[i + j]];
				tri.attributes[j] = new float[nAttrib];

				float *dest = tri.attributes[j];
				for (uint k = 0; k < streams.size(); k++){
					if (signed(k) != vertexStream){
						int nComp = streams[k].nComponents;
						memcpy(dest, streams[k].vertices + nComp * streams[k].indices[i + j], nComp * sizeof(float));
						dest += nComp;
					}
				}
			}

			triangles.push_back(tri);
		}
	}

	for (uint i = 0; i < triangles.size(); i++){

restart:

		uint prev = 2;
		for (uint j = 0; j < 3; j++){
			vec3 v0 = triangles[i].pos[prev];
			vec3 v1 = triangles[i].pos[j];

			for (uint k = 0; k < nVertices; k++){
				vec3 vVec = v1 - v0;
				vec3 lVec = vertices[k] - v0;

				float c = dot(lVec, vVec) / dot(vVec, vVec);
				if (c > 0.00001f && c < 0.99999f){
					vec3 pl = c * vVec - lVec;
					if (dot(pl, pl) < 0.0001f){

						// Copy current triangle
						Triangle tri;
						tri.batch = triangles[i].batch;
						for (uint x = 0; x < 3; x++){
							tri.pos[x] = triangles[i].pos[x];
							tri.attributes[x] = new float[nAttrib];
							memcpy(tri.attributes[x], triangles[i].attributes[x], nAttrib * sizeof(float));
						}

						// Assign new position for the splitted edge
						triangles[i].pos[j] = vertices[k];
						tri.pos[prev] = vertices[k];

						// Interpolate the other attributes
						for (uint x = 0; x < nAttrib; x++){
							float ip = lerp(tri.attributes[prev][x], tri.attributes[j][x], c);

							triangles[i].attributes[j][x] = ip;
							tri.attributes[prev][x] = ip;
						}

						triangles.push_back(tri);


						goto restart;
					}
				}
			}

			prev = j;
		}
	}

	std::sort(triangles.begin(), triangles.end(), compareTriangles);

	nIndices = 3 * triangles.size();

	uint currComp = 0;
	for (uint i = 0; i < streams.size(); i++){
		delete streams[i].vertices;
		delete streams[i].indices;

		if (signed(i) == vertexStream){
			streams[i].vertices = new float[nIndices * 3];
			for (uint j = 0; j < triangles.size(); j++){
				for (uint k = 0; k < 3; k++){
					((vec3 *) streams[i].vertices)[3 * j + k] = triangles[j].pos[k];
				}
			}
			
		} else {
			int nComp = streams[i].nComponents;

			streams[i].vertices = new float[nIndices * nComp];

			float *dest = streams[i].vertices;
			for (uint j = 0; j < triangles.size(); j++){
				for (uint k = 0; k < 3; k++){
					memcpy(dest, triangles[j].attributes[k] + currComp, nComp * sizeof(float));
					dest += nComp;
				}
			}

			currComp += nComp;
		}

		uint *indices = new uint[nIndices];
		for (uint j = 0; j < nIndices; j++){
			indices[j] = j;
		}

		streams[i].indices = indices;
		streams[i].nVertices = nIndices;
		streams[i].optimized = false;
	}

	// Fix the batches
	int currBatch = 0;
	uint i = 0;
	uint startIndex = 0;
	while (i < triangles.size()){
		while (i < triangles.size() && triangles[i].batch == currBatch){
			for (int j = 0; j < 3; j++){
				delete triangles[i].attributes[j];
			}
			i++;
		}

		batches[currBatch].startIndex = startIndex;
		batches[currBatch].nIndices = 3 * i - startIndex;
		startIndex = 3 * i;
		currBatch++;
	}
}

void Model::copyVertex(const uint destIndex, const Model &srcModel, const uint srcIndex){
	for (uint i = 0; i < streams.size(); i++){
		int nComp = streams[i].nComponents;
		float *dest = streams[i].vertices + nComp * streams[i].indices[destIndex];
		float *src = srcModel.streams[i].vertices + nComp * srcModel.streams[i].indices[srcIndex];

		memcpy(dest, src, nComp * sizeof(float));
	}
}

void Model::interpolateVertex(const uint destIndex, const Model &srcModel, const uint srcIndex0, const uint srcIndex1, const float x){
	for (uint i = 0; i < streams.size(); i++){
		int nComp = streams[i].nComponents;
		float *dest = streams[i].vertices + nComp * streams[i].indices[destIndex];
		float *src0 = srcModel.streams[i].vertices + nComp * srcModel.streams[i].indices[srcIndex0];
		float *src1 = srcModel.streams[i].vertices + nComp * srcModel.streams[i].indices[srcIndex1];
		
		for (int j = 0; j < nComp; j++){
			dest[j] = lerp(src0[j], src1[j], x);
		}
	}
}

bool Model::split(const vec3 &normal, const float offset, Model *front, Model *back) const {
	StreamID vertexStream = findStream(TYPE_VERTEX);
	if (vertexStream < 0 || streams[vertexStream].nComponents != 3) return false;

	vec3 *vertices = (vec3 *) streams[vertexStream].vertices;
	uint *indices = streams[vertexStream].indices;


	for (uint i = 0; i < streams.size(); i++){
		int nComp = streams[i].nComponents;
		if (front){
			front->addStream(streams[i].type, nComp, 0, new float[nComp * 2 * nIndices], NULL, false);
			front->streams[i].indices = getArrayIndices(nComp * 2 * nIndices);
		}
		if (back){
			back->addStream(streams[i].type, nComp, 0, new float[nComp * 2 * nIndices], NULL, false);
			back->streams[i].indices = getArrayIndices(nComp * 2 * nIndices);
		}
	}

	uint startFrontVertices = 0;
	uint startBackVertices  = 0;
	uint nFrontVertices = 0;
	uint nBackVertices  = 0;
	uint batch = 0;
	for (uint i = 0; i < nIndices; i += 3){
		while (batch < batches.size() && i >= batches[batch].startIndex + batches[batch].nIndices){
			if (front){
				front->addBatch(startFrontVertices, nFrontVertices - startFrontVertices);
				startFrontVertices = nFrontVertices;
			}
			if (back){
				back->addBatch(startBackVertices, nBackVertices - startBackVertices);
				startBackVertices = nBackVertices;
			}
			batch++;
		}

		
		vec3 v[3];
		float d[3];

		for (int j = 0; j < 3; j++){
			v[j] = vertices[indices[i + j]];
			d[j] = dot(v[j], normal) + offset;
		}

		if (d[0] >= 0 && d[1] >= 0 && d[2] >= 0){
			if (front){
				front->copyVertex(nFrontVertices++, *this, i);
				front->copyVertex(nFrontVertices++, *this, i + 1);
				front->copyVertex(nFrontVertices++, *this, i + 2);
			}
		} else if (d[0] < 0 && d[1] < 0 && d[2] < 0){
			if (back){
				back->copyVertex(nBackVertices++, *this, i);
				back->copyVertex(nBackVertices++, *this, i + 1);
				back->copyVertex(nBackVertices++, *this, i + 2);
			}
		} else {
			int front0 = 0;
			int prev = 2;
			for (int k = 0; k < 3; k++){
				if (d[prev] < 0 && d[k] >= 0){
					front0 = k;
					break;
				}
				prev = k;
			}

			int front1 = (front0 + 1) % 3;
			int front2 = (front0 + 2) % 3;
			if (d[front1] >= 0){
				float x0 = d[front1] / dot(normal, v[front1] - v[front2]);
				float x1 = d[front0] / dot(normal, v[front0] - v[front2]);

				if (front){
					front->copyVertex(nFrontVertices++, *this, i + front0);
					front->copyVertex(nFrontVertices++, *this, i + front1);
					front->interpolateVertex(nFrontVertices++, *this, i + front1, i + front2, x0);

					front->copyVertex(nFrontVertices++, *this, i + front0);
					front->interpolateVertex(nFrontVertices++, *this, i + front1, i + front2, x0);
					front->interpolateVertex(nFrontVertices++, *this, i + front0, i + front2, x1);
				}

				if (back){
					back->copyVertex(nBackVertices++, *this, i + front2);
					back->interpolateVertex(nBackVertices++, *this, i + front0, i + front2, x1);
					back->interpolateVertex(nBackVertices++, *this, i + front1, i + front2, x0);
				}

			} else {
				float x0 = d[front0] / dot(normal, v[front0] - v[front1]);
				float x1 = d[front0] / dot(normal, v[front0] - v[front2]);

				if (front){
					front->copyVertex(nFrontVertices++, *this, i + front0);
					front->interpolateVertex(nFrontVertices++, *this, i + front0, i + front1, x0);
					front->interpolateVertex(nFrontVertices++, *this, i + front0, i + front2, x1);
				}

				if (back){
					back->copyVertex(nBackVertices++, *this, i + front1);
					back->copyVertex(nBackVertices++, *this, i + front2);
					back->interpolateVertex(nBackVertices++, *this, i + front0, i + front2, x1);

					back->copyVertex(nBackVertices++, *this, i + front1);
					back->interpolateVertex(nBackVertices++, *this, i + front0, i + front2, x1);
					back->interpolateVertex(nBackVertices++, *this, i + front0, i + front1, x0);
				}
			}
		}
	}

	while (batch < batches.size()){
		if (front){
			front->addBatch(startFrontVertices, nFrontVertices - startFrontVertices);
			startFrontVertices = nFrontVertices;
		}
		if (back){
			back->addBatch(startBackVertices, nBackVertices - startBackVertices);
			startBackVertices = nBackVertices;
		}
		batch++;
	}

	for (uint i = 0; i < streams.size(); i++){
		if (front) front->streams[i].nVertices = nFrontVertices;
		if (back) back->streams[i].nVertices = nBackVertices;
	}
	if (front) front->setIndexCount(nFrontVertices);
	if (back) back->setIndexCount(nBackVertices);

	return true;
}

void Model::clear(){
	nIndices = 0;
	for (uint i = 0; i < streams.size(); i++){
		delete streams[i].vertices;
		delete streams[i].indices;
	}
	streams.clear();
	batches.clear();

	delete lastVertices;
	delete lastIndices;
	delete lastFormat;

	lastVertexCount = 0;
	lastVertices = NULL;
	lastIndices = NULL;
	lastFormat = NULL;
}

void Model::optimize(){
	for (uint i = 0; i < streams.size(); i++){
		optimizeStream(i);
	}
}

void Model::optimizeStream(const StreamID streamID){
	if (streams[streamID].optimized) return;

	uint nComp = streams[streamID].nComponents;
	uint nVert = streams[streamID].nVertices;

	KdTree <float> kdTree(nComp, nVert);

	uint *indexRemap = new uint[nVert];
	float *vertices = streams[streamID].vertices;
	for (uint i = 0; i < nVert; i++){
		float *vertex = vertices + i * nComp;
		uint index = kdTree.addUnique(vertex);

		indexRemap[i] = index;
		memcpy(vertices + index * nComp, vertex, nComp * sizeof(float));
	}

	uint *indices = streams[streamID].indices;
	for (uint j = 0; j < nIndices; j++){
		indices[j] = indexRemap[indices[j]];
	}

	delete indexRemap;
	streams[streamID].nVertices = kdTree.size();
	streams[streamID].vertices = (float *) realloc(vertices, kdTree.size() * nComp * sizeof(float));
	streams[streamID].optimized = true;
}

uint Model::assemble(const StreamID *aStreams, const uint nStreams, float **destVertices, uint **destIndices, bool separateArrays){
	uint i, j, nComp = getComponentCount(aStreams, nStreams);
	for (i = 0; i < nStreams; i++){
		optimizeStream(aStreams[i]);
	}

	if (separateArrays){
		for (i = 0; i < nStreams; i++){
			destVertices[i] = new float[streams[aStreams[i]].nComponents * nIndices];
		}
	} else {
		*destVertices = new float[nComp * nIndices];
	}
	uint *iDest = *destIndices = new uint[nIndices];

	uint *iIndex = new uint[nStreams];
	Hash hash(nStreams, nIndices >> 3, nIndices);
	for (j = 0; j < nIndices; j++){
		for (i = 0; i < nStreams; i++){
			iIndex[i] = streams[aStreams[i]].indices[j];
		}

		uint index;
		if (!hash.insert(iIndex, &index)){
			if (separateArrays){
				for (i = 0; i < nStreams; i++){
					uint nc = streams[aStreams[i]].nComponents;
					float *dest = destVertices[i] + index * nc;
					memcpy(dest, streams[aStreams[i]].vertices + streams[aStreams[i]].indices[j] * nc, nc * sizeof(float));
				}
			} else {
				float *dest = *destVertices + index * nComp;
				for (i = 0; i < nStreams; i++){
					uint nc = streams[aStreams[i]].nComponents;
					memcpy(dest, streams[aStreams[i]].vertices + streams[aStreams[i]].indices[j] * nc, nc * sizeof(float));
					dest += nc;
				}
			}
		}

		*iDest++ = index;
	}
	delete iIndex;

	if (separateArrays){
		for (i = 0; i < nStreams; i++){
			destVertices[i] = (float *) realloc(destVertices[i], hash.size() * streams[aStreams[i]].nComponents * sizeof(float));
		}
	} else {
		*destVertices = (float *) realloc(*destVertices, hash.size() * nComp * sizeof(float));
	}
	return hash.size();
}

void convertToShorts(const uint *src, int nIndices, const uint nVertices){
	unsigned short *dest = (unsigned short *) src;

/*	if (nVertices <= 32767 && cpuMMX){
		// MMX optimized int to short conversion
		while (nIndices >= 4){
			*((v4hi *) dest) = packssdw(*(v2si *) src, *(v2si *) (src + 2));
			src += 4;
			dest += 4;
			nIndices -= 4;
		}
		emms();
	}*/

	while (nIndices > 0){
		*dest++ = *src++;
		nIndices--;
	}
}

uint Model::makeDrawable(Renderer *renderer, const bool useCache, const ShaderID shader){
	if (streams.size() == 0) return 0;

	int vertexSize = getVertexSize();

	if (useCache && lastVertices){
		if ((vertexFormat = renderer->addVertexFormat(lastFormat, streams.size(), shader)) == VF_NONE) return 0;
		if ((vertexBuffer = renderer->addVertexBuffer(lastVertexCount * vertexSize, STATIC, lastVertices)) == VB_NONE) return 0;

		if (lastVertexCount > 65535){
			if ((indexBuffer = renderer->addIndexBuffer(nIndices, 4, STATIC, lastIndices)) == IB_NONE) return 0;
		} else {
			if ((indexBuffer = renderer->addIndexBuffer(nIndices, 2, STATIC, lastIndices)) == IB_NONE) return 0;
		}

		return lastVertexCount;
	} else {
		StreamID *aStreams = new StreamID[streams.size()];

		for (uint i = 0; i < streams.size(); i++){
			aStreams[i] = i;
		}
		float *vertices;
		uint *indices;

		uint nVertices = assemble(aStreams, streams.size(), &vertices, &indices, false);

		// Compute ranges for batches
		for (uint j = 0; j < batches.size(); j++){
			uint minVertex = 0xFFFFFFFF;
			uint maxVertex = 0;

			uint first = batches[j].startIndex;
			uint last  = first + batches[j].nIndices;
			if (first < last){
				for (uint i = first; i < last; i++){
					if (indices[i] < minVertex) minVertex = indices[i];
					if (indices[i] > maxVertex) maxVertex = indices[i];
				}

				batches[j].startVertex = minVertex;
				batches[j].nVertices = maxVertex - minVertex + 1;
			} else {
				// Empty batch
				batches[j].startVertex = 0;
				batches[j].nVertices = 0;
			}
		}

		FormatDesc *format = new FormatDesc[streams.size()];
		for (uint i = 0; i < streams.size(); i++){
			format[i].stream = 0;
			format[i].type   = streams[i].type;
			format[i].format = FORMAT_FLOAT;
			format[i].size   = streams[i].nComponents;
		}

		if ((vertexFormat = renderer->addVertexFormat(format, streams.size(), shader)) == VF_NONE) return 0;
		if ((vertexBuffer = renderer->addVertexBuffer(nVertices * vertexSize, STATIC, vertices)) == VB_NONE) return 0;

		if (nVertices > 65535){
			if ((indexBuffer = renderer->addIndexBuffer(nIndices, 4, STATIC, indices)) == IB_NONE) return 0;
		} else {
			convertToShorts(indices, nIndices, nVertices);

			if ((indexBuffer = renderer->addIndexBuffer(nIndices, 2, STATIC, indices)) == IB_NONE) return 0;
		}

		delete aStreams;

		if (useCache){
			delete lastFormat;
			delete lastVertices;
			delete lastIndices;

			lastFormat = format;
			lastVertexCount = nVertices;
			lastVertices = vertices;
			lastIndices = indices;
		} else {
			delete format;
			delete vertices;
			delete indices;
		}

		return nVertices;
	}
}

void Model::unmakeDrawable(Renderer *renderer){
//	renderer->removeVertexFormat(indexBuffer);
//	renderer->removeVertexBuffer(vertexBuffer);
//	renderer->removeIndexBuffer(indexBuffer);
}

void Model::setBuffers(Renderer *renderer){
	renderer->setVertexFormat(vertexFormat);
	renderer->setVertexBuffer(0, vertexBuffer);
	renderer->setIndexBuffer(indexBuffer);
}

void Model::draw(Renderer *renderer){
	ASSERT(vertexBuffer != VB_NONE);
	ASSERT(indexBuffer  != IB_NONE);

	renderer->changeVertexFormat(vertexFormat);
	renderer->changeVertexBuffer(0, vertexBuffer);
	renderer->changeIndexBuffer(indexBuffer);

	renderer->drawElements(PRIM_TRIANGLES, 0, nIndices, 0, lastVertexCount);
}

void Model::drawBatch(Renderer *renderer, const uint batch){
	ASSERT(vertexBuffer != VB_NONE);
	ASSERT(indexBuffer  != IB_NONE);

	renderer->changeVertexFormat(vertexFormat);
	renderer->changeVertexBuffer(0, vertexBuffer);
	renderer->changeIndexBuffer(indexBuffer);

	renderer->drawElements(PRIM_TRIANGLES, batches[batch].startIndex, batches[batch].nIndices, batches[batch].startVertex, batches[batch].nVertices);
}

void Model::drawSubBatch(Renderer *renderer, const uint batch, const uint first, const uint count){
	ASSERT(vertexBuffer != VB_NONE);
	ASSERT(indexBuffer  != IB_NONE);

	renderer->changeVertexFormat(vertexFormat);
	renderer->changeVertexBuffer(0, vertexBuffer);
	renderer->changeIndexBuffer(indexBuffer);

	int startIndex = batches[batch].startIndex + first;
	int indexCount = min(count, batches[batch].nIndices - first);

	renderer->drawElements(PRIM_TRIANGLES, startIndex, indexCount, batches[batch].startVertex, batches[batch].nVertices);
}

uint *Model::getArrayIndices(const uint nVertices){
	uint *indices = new uint[nVertices];
	for (uint i = 0; i < nVertices; i++){
		indices[i] = i;
	}
	return indices;
}