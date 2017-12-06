#ifndef _DIRECT3D11RENDERER_H_
#define _DIRECT3D11RENDERER_H_

#include "../Renderer.h"
#include <d3d11.h>

#define SAFE_RELEASE(p) { if (p){ p->Release(); p = NULL; } }

#define ROLLING_VB_SIZE (64 * 1024)

/*
#define VB_INVALID (-2)
*/


class Direct3D11Renderer : public Renderer
{
public:
	Direct3D11Renderer();
	Direct3D11Renderer(ID3D11Device *d3ddev, ID3D11DeviceContext *ctx);
	~Direct3D11Renderer();

	void Initialize();

	void resetToDefaults();
	void reset(const uint flags = RESET_ALL);

//	bool resetDevice();

	TextureID addTexture(ID3D11Resource *resource, uint flags = 0);
	TextureID addTexture(Image &img, const SamplerStateID samplerState = SS_NONE, uint flags = 0);

	TextureID addRenderTarget(const int width, const int height, const int depth, const int mipMapCount, const int arraySize, const FORMAT format, const int msaaSamples = 1, const SamplerStateID samplerState = SS_NONE, uint flags = 0);
	TextureID addRenderDepth(const int width, const int height, const int arraySize, const FORMAT format, const int msaaSamples = 1, const SamplerStateID samplerState = SS_NONE, uint flags = 0);

	bool resizeRenderTarget(const TextureID renderTarget, const int width, const int height, const int depth, const int mipMapCount, const int arraySize);
	bool generateMipMaps(const TextureID renderTarget);

	void removeTexture(const TextureID texture);

	ShaderID addShader(const char *vsText, const char *gsText, const char *fsText, const char *csText, const int vsLine, const int gsLine, const int fsLine, const int csLine,
		const char *header = NULL, const char *extra = NULL, const char *fileName = NULL, const char **attributeNames = NULL, const int nAttributes = 0, const uint flags = 0);
	VertexFormatID addVertexFormat(const FormatDesc *formatDesc, const uint nAttribs, const ShaderID shader = SHADER_NONE);
	VertexBufferID addVertexBuffer(const long size, const BufferAccess bufferAccess, const void *data = NULL);
	IndexBufferID addIndexBuffer(const uint nIndices, const uint indexSize, const BufferAccess bufferAccess, const void *data = NULL);
	IndexBufferID addIndexBuffer(ID3D11Buffer* buffer, const uint nIndices, const uint indexSize);

	SamplerStateID addSamplerState(const Filter filter, const AddressMode s, const AddressMode t, const AddressMode r, const float lod = 0, const uint maxAniso = 16, const int compareFunc = 0, const float *border_color = NULL);
	BlendStateID addBlendState(const BlendOp srcFactorRGB, const BlendOp destFactorRGB, const BlendOp srcFactorAlpha, const BlendOp destFactorAlpha, const BlendMode blendModeRGB, const BlendMode blendModeAlpha, const int mask = ALL, const bool alphaToCoverage = false);
	DepthStateID addDepthState(const bool depthTest, const bool depthWrite, const CompareMode depthFunc, const bool stencilTest, const uint8 stencilReadMask, const uint8 stencilWriteMask,
		const CompareMode stencilFuncFront, const CompareMode stencilFuncBack, const StencilOp stencilFailFront, const StencilOp stencilFailBack,
		const StencilOp depthFailFront, const StencilOp depthFailBack, const StencilOp stencilPassFront, const StencilOp stencilPassBack);
	RasterizerStateID addRasterizerState(CullMode cullMode, const FillMode fillMode = FillMode::SOLID, const bool multiSample = true, const bool scissor = false, const float depthBias = 0.0f, const float slopeDepthBias = 0.0f);

	void setTexture(const char *textureName, const TextureID texture);
	void setTexture(const char *textureName, const TextureID texture, const SamplerStateID samplerState);
	void setTextureSlice(const char *textureName, const TextureID texture, const int slice);
	void applyTextures();

	void setSamplerState(const char *samplerName, const SamplerStateID samplerState);
	void applySamplerStates();

	void setShaderConstantRaw(const char *name, const void *data, const int size);
	void applyConstants();

//	void changeTexture(const uint imageUnit, const TextureID textureID);
	void changeRenderTargets(const TextureID *colorRTs, const uint nRenderTargets, const TextureID depthRT = TEXTURE_NONE, const int depthSlice = NO_SLICE, const int *slices = NULL);
	void changeToMainFramebuffer();
	void changeShader(const ShaderID shaderID);
	void changeVertexFormat(const VertexFormatID vertexFormatID);
	void changeVertexBuffer(const int stream, const VertexBufferID vertexBufferID, const intptr offset = 0);
	void changeIndexBuffer(const IndexBufferID indexBufferID);
	void changeCullFace(const int cullFace);

//	void changeSamplerState(const uint samplerUnit, const SamplerStateID samplerState);
	void changeBlendState(const BlendStateID blendState, const uint sampleMask = ~0);
	void changeDepthState(const DepthStateID depthState, const uint stencilRef = 0);
	void changeRasterizerState(const RasterizerStateID rasterizerState);


	void clear(const bool clearColor, const bool clearDepth, const bool clearStencil, const float *color, const float depth, const uint stencil);
	void clearRenderTarget(const TextureID renderTarget, const float4 &color, const int slice = NO_SLICE);
	void clearDepthTarget(const TextureID depthTarget, const float depth = 1.0f, const int slice = NO_SLICE);

	void drawArrays(const Primitives primitives, const int firstVertex, const int nVertices);
	void drawElements(const Primitives primitives, const int firstIndex, const int nIndices, const int firstVertex, const int nVertices);

	void setup2DMode(const float left, const float right, const float top, const float bottom);
	void drawPlain(const Primitives primitives, vec2 *vertices, const uint nVertices, const BlendStateID blendState, const DepthStateID depthState, const vec4 *color = NULL);
	void drawTextured(const Primitives primitives, TexVertex *vertices, const uint nVertices, const TextureID texture, const SamplerStateID samplerState, const BlendStateID blendState, const DepthStateID depthState, const vec4 *color = NULL);

	void setFrameBuffer(ID3D11RenderTargetView *colorRTV, ID3D11DepthStencilView *depthDSV)
	{
		backBufferRTV  = colorRTV;
		depthBufferDSV = depthDSV;
	}

	ID3D11Resource *getResource(const TextureID texture) const;
	ID3D11Resource *getVBResource(const VertexBufferID buffer) const;
	ID3D11Resource *getIBResource(const IndexBufferID buffer) const;

	void flush();
	void finish();

protected:
	ID3D11ShaderResourceView *createSRV(ID3D11Resource *resource, DXGI_FORMAT format = DXGI_FORMAT_UNKNOWN, const int firstSlice = -1, const int sliceCount = -1);
	ID3D11RenderTargetView   *createRTV(ID3D11Resource *resource, DXGI_FORMAT format = DXGI_FORMAT_UNKNOWN, const int firstSlice = -1, const int sliceCount = -1);
	ID3D11DepthStencilView   *createDSV(ID3D11Resource *resource, DXGI_FORMAT format = DXGI_FORMAT_UNKNOWN, const int firstSlice = -1, const int sliceCount = -1);

	ID3D11Device *device;
	ID3D11DeviceContext *context;
	ID3D11RenderTargetView *backBufferRTV;
	ID3D11DepthStencilView *depthBufferDSV;

	TextureID currentTexturesVS[MAX_TEXTUREUNIT], selectedTexturesVS[MAX_TEXTUREUNIT];
	TextureID currentTexturesGS[MAX_TEXTUREUNIT], selectedTexturesGS[MAX_TEXTUREUNIT];
	TextureID currentTexturesPS[MAX_TEXTUREUNIT], selectedTexturesPS[MAX_TEXTUREUNIT];
	int currentTextureSlicesVS[MAX_TEXTUREUNIT], selectedTextureSlicesVS[MAX_TEXTUREUNIT];
	int currentTextureSlicesGS[MAX_TEXTUREUNIT], selectedTextureSlicesGS[MAX_TEXTUREUNIT];
	int currentTextureSlicesPS[MAX_TEXTUREUNIT], selectedTextureSlicesPS[MAX_TEXTUREUNIT];

	SamplerStateID currentSamplerStatesVS[MAX_SAMPLERSTATE], selectedSamplerStatesVS[MAX_SAMPLERSTATE];
	SamplerStateID currentSamplerStatesGS[MAX_SAMPLERSTATE], selectedSamplerStatesGS[MAX_SAMPLERSTATE];
	SamplerStateID currentSamplerStatesPS[MAX_SAMPLERSTATE], selectedSamplerStatesPS[MAX_SAMPLERSTATE];

private:
	ubyte *mapRollingVB(const uint size);
	void unmapRollingVB(const uint size);
	uint copyToRollingVB(const void *src, const uint size);

	VertexBufferID rollingVB;
	int rollingVBOffset;

	ShaderID plainShader, texShader;
	VertexFormatID plainVF, texVF;

	float4 scaleBias2D;

	ID3D11Query *eventQuery;
};

#endif // _DIRECT3D10RENDERER_H_
