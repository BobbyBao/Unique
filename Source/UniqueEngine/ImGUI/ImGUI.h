#pragma once

struct nk_font_atlas;

namespace Unique
{
	struct NkImpl;

	struct UIVS
	{
		Matrix4 UIProj;
	};

	class GUI : public Object
	{
	public:
		GUI();
		~GUI();
	private:
		void HandleStartup(const struct Startup& eventData);
		void FontStashBegin(nk_font_atlas **atlas);
		void FontStashEnd(void);
		void HandleInputBegin(const struct InputBegin& eventData);
		void HandleInputEnd(const struct InputEnd& eventData);
		void HandleSDLRawInput(const struct SDLRawInput& eventData);
		void HandleBeginFrame(const struct BeginFrame& eventData);
		void HandlePostRenderUpdate(const struct PostRenderUpdate& eventData);
		
		NkImpl& impl_;

		SPtr<Geometry> geometry_;
		SPtr<VertexBuffer> vertexBuffer_;
		SPtr<IndexBuffer> indexBuffer_;
		SPtr<UniformBuffer> uiConstants_;
		SPtr<Material> material_;
		SPtr<PipelineState> pipeline_;
		SPtr<Texture> font_texture;
	};
}
