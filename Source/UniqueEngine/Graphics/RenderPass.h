#pragma once


namespace Unique
{
	/// Rendering path command types.
	enum RenderPassType
	{
		CMD_SCENEPASS,
		CMD_QUAD,
		CMD_FORWARDLIGHTS,
		CMD_LIGHTVOLUMES,
		CMD_RENDERUI,
		CMD_SENDEVENT
	};

	/// Rendering path sorting modes.
	enum RenderPassSortMode
	{
		SORT_FRONTTOBACK = 0,
		SORT_BACKTOFRONT
	};

	/// Rendering path command.
	class UNIQUE_API RenderPass : public Object
	{
		uRTTI(RenderPass, Object)
	public:
		/// Construct.
		RenderPass(RenderPassType type);
		/// Set a texture resource name. Can also refer to a rendertarget defined in the rendering path.
		void SetTextureName(TextureUnit unit, const String& name);
		/// Set number of output rendertargets.
		void SetNumOutputs(unsigned num);
		/// Set output rendertarget name and face index for cube maps.
		void SetOutput(unsigned index, const String& name, CubeMapFace face = FACE_POSITIVE_X);
		/// Set output rendertarget name.
		void SetOutputName(unsigned index, const String& name);
		/// Set output rendertarget face index for cube maps.
		void SetOutputFace(unsigned index, CubeMapFace face);
		/// Set depth-stencil output name. When empty, will assign a depth-stencil buffer automatically.
		void SetDepthStencilName(const String& name);

		/// Return texture resource name.
		const String& GetTextureName(TextureUnit unit) const;

		/// Return number of output rendertargets.
		unsigned GetNumOutputs() const { return (uint)outputs_.size(); }

		/// Return output rendertarget name.
		const String& GetOutputName(unsigned index) const;
		/// Return output rendertarget face index.
		CubeMapFace GetOutputFace(unsigned index) const;

		/// Return depth-stencil output name.
		const String& GetDepthStencilName() const { return depthStencilName_; }

		void Update();
		void Render(View* view);

		/// Scene pass name.
		String pass_;
		/// Command type.
		RenderPassType type_;
		/// Scene pass index. Filled by View.
		unsigned passIndex_;
		/// Enabled flag.
		bool enabled_;
		/// Tag name.
		String tag_;
		/// Sorting mode.
		RenderPassSortMode sortMode_;
	protected:
		virtual void OnUpdate();
		virtual void OnRender(View* view);

		Graphics& graphics_;
		/// Command/pass metadata.
		String metadata_;
		/// Vertex shader name.
		String vertexShaderName_;
		/// Pixel shader name.
		String pixelShaderName_;
		/// Vertex shader defines.
		String vertexShaderDefines_;
		/// Pixel shader defines.
		String pixelShaderDefines_;
		/// Textures.
		String textureNames_[MAX_TEXTURE_UNITS];
		/// Textures.
		HashMap<String, SPtr<GraphicsBuffer>> buffers_;
		/// Textures.
		HashMap<String, SPtr<Texture>> textures_;
		/// Output rendertarget names and faces.
		Vector<Pair<String, CubeMapFace> > outputs_;
		/// Depth-stencil output name.
		String depthStencilName_;
		/// Clear flags. Affects clear command only.
		unsigned clearFlags_;
		/// Clear color. Affects clear command only.
		Color clearColor_;
		/// Clear depth. Affects clear command only.
		float clearDepth_;
		/// Clear stencil value. Affects clear command only.
		unsigned clearStencil_;
		/// Blend mode. Affects quad command only.
		//BlendMode blendMode_;
		/// Use fog color for clearing.
		bool useFogColor_;
		/// Mark to stencil flag.
		bool markToStencil_;
		/// Use lit base pass optimization for forward per-pixel lights.
		bool useLitBase_;
		/// Vertex lights flag.
		bool vertexLights_;
		/// Event name.
		String eventName_;
	};

	class ScenePass : public RenderPass
	{
	public:
		ScenePass(unsigned clearFlags = CLEAR_COLOR | CLEAR_DEPTH);

		virtual void OnRender(View* view);
	};

}