#pragma once
#include "../Graphics/Drawable.h"
#include "../Graphics/Material.h"
#include "Math/MathDefs.h"
#include "Math/Matrix3x4.h"
#include "Math/Rect.h"

namespace Unique
{
	class Camera;
	class Drawable;
	class Geometry;
	class Light;
	class Material;
	class Matrix3x4;
	class Pass;
	class ShaderVariation;
	class Texture;
	class VertexBuffer;
	class View;
	struct BatchQueue;
	struct LightBatchQueue;

	/// Queued 3D geometry draw call.
	struct Batch
	{
		/// Construct with defaults.
		Batch() : isBase_(false)
		{
		}

		/// Construct from a drawable's source batch.
		Batch(const SourceBatch& rhs);
		
		/// Construct from transient buffer.
		Batch(Geometry* geometry, Material* material, const Matrix3x4* worldTransform = nullptr);

		/// Calculate state sorting key, which consists of base pass flag, light, pass and geometry.
		void CalculateSortKey();
		/// Prepare for rendering.
		void Prepare(View* view, Camera* camera, bool setModelTransform) const;
		/// Prepare and draw.
		void Draw(View* view, Camera* camera) const;

		/// State sorting key.
		unsigned long long sortKey_;
		/// Distance from camera.
		float distance_;
		/// %Geometry type.
		GeometryType geometryType_;
		/// 8-bit render order modifier from material.
		unsigned char renderOrder_;
		/// 8-bit light mask for stencil marking in deferred rendering.
		unsigned char lightMask_;
		/// Base batch flag. This tells to draw the object fully without light optimizations.
		bool isBase_;

		PrimitiveTopology primitiveTopology_;
		uint vertexOffset_;
		uint vertexCount_;
		uint indexOffset_;
		uint indexCount_;

		/// Geometry.
		Geometry* geometry_ = nullptr;
		/// Material.
		Material* material_ = nullptr;

		union
		{
			/// World transform(s). For a skinned model, these are the bone transforms.
			const Matrix3x4* worldTransform_;	
			// Offset in batchMatrics
			size_t transformOffset_ = 0;
		};
		/// Number of world transforms.
		unsigned numWorldTransforms_;
		/// Per-instance data. If not null, must contain enough data to fill instancing buffer.
		void* instancingData_;
		/// Material pass.
		Pass* pass_;

		PipelineState*  pipelineState_;
	};


	/// Data for one geometry instance.
	struct InstanceData
	{
		/// Construct undefined.
		InstanceData()
		{
		}

		/// Construct with transform, instancing data and distance.
		InstanceData(const Matrix3x4* worldTransform, const void* instancingData, float distance) :
			worldTransform_(worldTransform),
			instancingData_(instancingData),
			distance_(distance)
		{
		}

		/// World transform.
		const Matrix3x4* worldTransform_;
		/// Instancing data buffer.
		const void* instancingData_;
		/// Distance from camera.
		float distance_;
	};

	/// Instanced 3D geometry draw call.
	struct BatchGroup : public Batch
	{
		/// Construct with defaults.
		BatchGroup() :
			startIndex_(M_MAX_UNSIGNED)
		{
		}

		/// Construct from a batch.
		BatchGroup(const Batch& batch) :
			Batch(batch),
			startIndex_(M_MAX_UNSIGNED)
		{
		}

		/// Destruct.
		~BatchGroup()
		{
		}

		/// Add world transform(s) from a batch.
		void AddTransforms(const Batch& batch)
		{
			InstanceData newInstance;
			newInstance.distance_ = batch.distance_;
			newInstance.instancingData_ = batch.instancingData_;

			for (unsigned i = 0; i < batch.numWorldTransforms_; ++i)
			{
				newInstance.worldTransform_ = &batch.worldTransform_[i];
				instances_.push_back(newInstance);
			}
		}

		/// Pre-set the instance data. Buffer must be big enough to hold all data.
		void SetInstancingData(void* lockedData, unsigned stride, unsigned& freeIndex);
		/// Prepare and draw.
		void Draw(View* view, Camera* camera) const;

		/// Instance data.
		PODVector<InstanceData> instances_;
		/// Instance stream start index, or M_MAX_UNSIGNED if transforms not pre-set.
		unsigned startIndex_;
	};

	/// Instanced draw call grouping key.
	struct BatchGroupKey
	{
		/// Construct undefined.
		BatchGroupKey()
		{
		}

		/// Construct from a batch.
		BatchGroupKey(const Batch& batch) :
			//zone_(batch.zone_),
			//lightQueue_(batch.lightQueue_),
			pass_(batch.pass_),
			material_(batch.material_),
			geometry_(batch.geometry_),
			renderOrder_(batch.renderOrder_)
		{
		}

		/// Zone.
		//Zone* zone_;
		/// Light properties.
		LightBatchQueue* lightQueue_ = nullptr;
		/// Material pass.
		Pass* pass_;
		/// Material.
		Material* material_;
		/// Geometry.
		Geometry* geometry_;
		/// 8-bit render order modifier from material.
		unsigned char renderOrder_;

		/// Test for equality with another batch group key.
		bool operator ==(const BatchGroupKey& rhs) const
		{
			return /*zone_ == rhs.zone_ &&*/ lightQueue_ == rhs.lightQueue_ && pass_ == rhs.pass_ && material_ == rhs.material_ &&
				geometry_ == rhs.geometry_ && renderOrder_ == rhs.renderOrder_;
		}

		/// Test for inequality with another batch group key.
		bool operator !=(const BatchGroupKey& rhs) const
		{
			return /*zone_ != rhs.zone_ ||*/ lightQueue_ != rhs.lightQueue_ || pass_ != rhs.pass_ || material_ != rhs.material_ ||
				geometry_ != rhs.geometry_ || renderOrder_ != rhs.renderOrder_;
		}

		/// Return hash value.
		unsigned ToHash() const;
	};
}

#include <xstddef>
namespace std
{
	template<>
	struct hash<Unique::BatchGroupKey>
	{
		size_t operator()(const Unique::BatchGroupKey& value) const
		{
			return value.ToHash();
		}
	};
}


namespace Unique
{

	/// Queue that contains both instanced and non-instanced draw calls.
	struct BatchQueue
	{
	public:
		/// Clear for new frame by clearing all groups and batches.
		void Clear(int maxSortedInstances);
		/// Sort non-instanced draw calls back to front.
		void SortBackToFront();
		/// Sort instanced and non-instanced draw calls front to back.
		void SortFrontToBack();
		/// Sort batches front to back while also maintaining state sorting.
		void SortFrontToBack2Pass(PODVector<Batch*>& batches);
		/// Pre-set instance data of all groups. The vertex buffer must be big enough to hold all data.
		void SetInstancingData(void* lockedData, unsigned stride, unsigned& freeIndex);
		/// Draw.
		void Draw(View* view, Camera* camera) const;
		/// Return the combined amount of instances.
		size_t GetNumInstances(View* view);

		/// Return whether the batch group is empty.
		bool IsEmpty() const { return batches_.empty() && batchGroups_.empty(); }

		/// Instanced draw calls.
		HashMap<BatchGroupKey, BatchGroup> batchGroups_;
		/// Unsorted non-instanced draw calls.
		PODVector<Batch> batches_;
		/// Sorted non-instanced draw calls.
		PODVector<Batch*> sortedBatches_;
		/// Sorted instanced draw calls.
		PODVector<BatchGroup*> sortedBatchGroups_;
		/// Maximum sorted instances.
		unsigned maxSortedInstances_;
		/// Whether the pass command contains extra shader defines.
		bool hasExtraDefines_;
		/// Vertex shader extra defines.
		String vsExtraDefines_;
		/// Pixel shader extra defines.
		String psExtraDefines_;
		/// Hash for vertex shader extra defines.
		StringID vsExtraDefinesHash_;
		/// Hash for pixel shader extra defines.
		StringID psExtraDefinesHash_;
	};

	/// Queue for shadow map draw calls
	struct ShadowBatchQueue
	{
		/// Shadow map camera.
		Camera* shadowCamera_;
		/// Shadow map viewport.
		IntRect shadowViewport_;
		/// Shadow caster draw calls.
		BatchQueue shadowBatches_;
		/// Directional light cascade near split distance.
		float nearSplit_;
		/// Directional light cascade far split distance.
		float farSplit_;
	};

	/// Queue for light related draw calls.
	struct LightBatchQueue
	{
		/// Per-pixel light.
		Light* light_;
		/// Light negative flag.
		bool negative_;
		/// Shadow map depth texture.
		Texture* shadowMap_;
		/// Lit geometry draw calls, base (replace blend mode)
		BatchQueue litBaseBatches_;
		/// Lit geometry draw calls, non-base (additive)
		BatchQueue litBatches_;
		/// Shadow map split queues.
		Vector<ShadowBatchQueue> shadowSplits_;
		/// Per-vertex lights.
		PODVector<Light*> vertexLights_;
		/// Light volume draw calls.
		PODVector<Batch> volumeBatches_;
	};

}
