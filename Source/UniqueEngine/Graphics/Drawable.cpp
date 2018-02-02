#include "Precompiled.h"
#include "Core/Context.h"
#include "../Graphics/Camera.h"
#include "../graphics/light.h"
#include "../Graphics/DebugRenderer.h"
#include "IO/File.h"
#include "../Graphics/Graphics.h"
#include "../Graphics/Geometry.h"
#include "../Graphics/Material.h"
#include "../Graphics/Octree.h"
#include "../Graphics/VertexBuffer.h"
#include "../Graphics/Zone.h"
#include "IO/Log.h"
#include "../Scene/Scene.h"
#include "DebugNew.h"

#ifdef _MSC_VER
#pragma warning(disable:6293)
#endif

namespace Unique
{

const char* GEOMETRY_CATEGORY = "Geometry";

SourceBatch::SourceBatch() :
    distance_(0.0f),
    geometry_(0),
    worldTransform_(&Matrix3x4::IDENTITY),
    numWorldTransforms_(1),
    instancingData_((void*)0),
    geometryType_(GEOM_STATIC)
{
}

SourceBatch::SourceBatch(const SourceBatch& batch)
{
    *this = batch;
}

SourceBatch::~SourceBatch()
{
}

SourceBatch& SourceBatch::operator =(const SourceBatch& rhs)
{
    distance_ = rhs.distance_;
    geometry_ = rhs.geometry_;
    material_ = rhs.material_;
    worldTransform_ = rhs.worldTransform_;
    numWorldTransforms_ = rhs.numWorldTransforms_;
    instancingData_ = rhs.instancingData_;
    geometryType_ = rhs.geometryType_;
	pipelineState_ = rhs.pipelineState_;
	primitiveTopology_ = rhs.primitiveTopology_;
	vertexOffset_ = rhs.vertexOffset_;
	vertexCount_ = rhs.vertexCount_;
	indexOffset_ = rhs.indexOffset_;
	indexCount_ = rhs.indexCount_;
    return *this;
}

uObject(Drawable)
{
	uAttribute("View Mask", viewMask_);
	uAttribute("Light Mask", lightMask_);
	uAttribute("Shadow Mask", shadowMask_);
}

Drawable::Drawable(unsigned char drawableFlags) :
    boundingBox_(0.0f, 0.0f),
    drawableFlags_(drawableFlags),
    worldBoundingBoxDirty_(true),
    castShadows_(false),
    occluder_(false),
    occludee_(true),
    updateQueued_(false),
    zoneDirty_(false),
    octant_(0),
    zone_(0),
    viewMask_(DEFAULT_VIEWMASK),
    lightMask_(DEFAULT_LIGHTMASK),
    shadowMask_(DEFAULT_SHADOWMASK),
    zoneMask_(DEFAULT_ZONEMASK),
    viewFrameNumber_(0),
    distance_(0.0f),
    lodDistance_(0.0f),
    drawDistance_(0.0f),
    shadowDistance_(0.0f),
    sortValue_(0.0f),
    minZ_(0.0f),
    maxZ_(0.0f),
    lodBias_(1.0f),
    basePassFlags_(0),
    maxLights_(0)
{
}

Drawable::~Drawable()
{
    RemoveFromOctree();
}

void Drawable::OnSetEnabled()
{
    bool enabled = IsEnabledEffective();

    if (enabled && !octant_)
        AddToOctree();
    else if (!enabled && octant_)
        RemoveFromOctree();
}

void Drawable::ProcessRayQuery(const RayOctreeQuery& query, PODVector<RayQueryResult>& results)
{
    float distance = query.ray_.HitDistance(GetWorldBoundingBox());
    if (distance < query.maxDistance_)
    {
        RayQueryResult result;
        result.position_ = query.ray_.origin_ + distance * query.ray_.direction_;
        result.normal_ = -query.ray_.direction_;
        result.distance_ = distance;
        result.drawable_ = this;
        result.node_ = GetNode();
        result.subObject_ = M_MAX_UNSIGNED;
        results.push_back(result);
    }
}

void Drawable::UpdateBatches(const FrameInfo& frame)
{
    const BoundingBox& worldBoundingBox = GetWorldBoundingBox();
    const Matrix3x4& worldTransform = node_->GetWorldTransform();
    distance_ = frame.camera_->GetDistance(worldBoundingBox.Center());

    for (unsigned i = 0; i < batches_.size(); ++i)
    {
        batches_[i].distance_ = distance_;
        batches_[i].worldTransform_ = &worldTransform;
    }

    float scale = worldBoundingBox.Size().DotProduct(DOT_SCALE);
    float newLodDistance = frame.camera_->GetLodDistance(distance_, scale, lodBias_);

    if (newLodDistance != lodDistance_)
        lodDistance_ = newLodDistance;
}

Geometry* Drawable::GetLodGeometry(unsigned batchIndex, unsigned level)
{
    // By default return the visible batch geometry
    if (batchIndex < batches_.size())
        return batches_[batchIndex].geometry_;
    else
        return 0;
}

bool Drawable::DrawOcclusion(OcclusionBuffer* buffer)
{
    return true;
}

void Drawable::DrawDebugGeometry(DebugRenderer* debug, bool depthTest)
{
     if (debug && IsEnabledEffective())
         debug->AddBoundingBox(GetWorldBoundingBox(), Color::GREEN, depthTest);
}

void Drawable::SetDrawDistance(float distance)
{
    drawDistance_ = distance;
}

void Drawable::SetShadowDistance(float distance)
{
    shadowDistance_ = distance;
}

void Drawable::SetLodBias(float bias)
{
    lodBias_ = Max(bias, M_EPSILON);
}

void Drawable::SetViewMask(unsigned mask)
{
    viewMask_ = mask;
}

void Drawable::SetLightMask(unsigned mask)
{
    lightMask_ = mask;
}

void Drawable::SetShadowMask(unsigned mask)
{
    shadowMask_ = mask;
}

void Drawable::SetZoneMask(unsigned mask)
{
    zoneMask_ = mask;
    // Mark dirty to reset cached zone
    OnMarkedDirty(node_);
}

void Drawable::SetMaxLights(unsigned num)
{
    maxLights_ = num;
}

void Drawable::SetCastShadows(bool enable)
{
    castShadows_ = enable;
}

void Drawable::SetOccluder(bool enable)
{
    occluder_ = enable;
}

void Drawable::SetOccludee(bool enable)
{
    if (enable != occludee_)
    {
        occludee_ = enable;
        // Reinsert to octree to make sure octant occlusion does not erroneously hide this drawable
        if (octant_ && !updateQueued_)
            octant_->GetRoot()->QueueUpdate(this);
    
    }
}

void Drawable::MarkForUpdate()
{
    if (!updateQueued_ && octant_)
        octant_->GetRoot()->QueueUpdate(this);
}

const BoundingBox& Drawable::GetWorldBoundingBox()
{
    if (worldBoundingBoxDirty_)
    {
        OnWorldBoundingBoxUpdate();
        worldBoundingBoxDirty_ = false;
    }

    return worldBoundingBox_;
}
#if false
bool Drawable::IsInView() const
{
    // Note: in headless mode there is no renderer subsystem and no view frustum tests are performed, so return
    // always false in that case
    Renderer* renderer = Subsystem<Renderer>();
    return renderer && viewFrameNumber_ == renderer->GetFrameInfo().frameNumber_ && !viewCameras_.empty();
}

bool Drawable::IsInView(Camera* camera) const
{
    Renderer* renderer = Subsystem<Renderer>();
    return renderer && viewFrameNumber_ == renderer->GetFrameInfo().frameNumber_ && (!camera || viewCameras_.Contains(camera));
}

bool Drawable::IsInView(const FrameInfo& frame, bool anyCamera) const
{
    return viewFrameNumber_ == frame.frameNumber_ && (anyCamera || viewCameras_.Contains(frame.camera_));
}

#endif

void Drawable::SetZone(Zone* zone, bool temporary)
{
    zone_ = zone;

    // If the zone assignment was temporary (inconclusive) set the dirty flag so that it will be re-evaluated on the next frame
    zoneDirty_ = temporary;
}

void Drawable::SetSortValue(float value)
{
    sortValue_ = value;
}

void Drawable::MarkInView(const FrameInfo& frame)
{
    if (frame.frameNumber_ != viewFrameNumber_)
    {
        viewFrameNumber_ = frame.frameNumber_;
        viewCameras_.resize(1);
        viewCameras_[0] = frame.camera_;
    }
    else
        viewCameras_.push_back(frame.camera_);

    basePassFlags_ = 0;
}

void Drawable::MarkInView(unsigned frameNumber)
{
    if (frameNumber != viewFrameNumber_)
    {
        viewFrameNumber_ = frameNumber;
        viewCameras_.clear();
    }
}

void Drawable::OnNodeSet(Node* node)
{
    if (node)
        node->AddListener(this);
}

void Drawable::OnSceneSet(Scene* scene)
{
    if (scene)
        AddToOctree();
    else
        RemoveFromOctree();
}

void Drawable::OnMarkedDirty(Node* node)
{
    worldBoundingBoxDirty_ = true;
    if (!updateQueued_ && octant_)
        octant_->GetRoot()->QueueUpdate(this);

    // Mark zone assignment dirty when transform changes
    if (node == node_)
        zoneDirty_ = true;
}

void Drawable::AddToOctree()
{
    // Do not add to octree when disabled
    if (!IsEnabledEffective())
        return;

    Scene* scene = GetScene();
    if (scene)
    {
        Octree* octree = scene->GetComponent<Octree>();
        if (octree)
            octree->InsertDrawable(this);
        else
            UNIQUE_LOGERROR("No Octree component in scene, drawable will not render");
    }
    else
    {
        // We have a mechanism for adding detached nodes to an octree manually, so do not log this error
        //UNIQUE_LOGERROR("Node is detached from scene, drawable will not render");
    }
}

void Drawable::RemoveFromOctree()
{
    if (octant_)
    {
        Octree* octree = octant_->GetRoot();
        if (updateQueued_)
            octree->CancelUpdate(this);

        // Perform subclass specific deinitialization if necessary
        OnRemoveFromOctree();

        octant_->RemoveDrawable(this);
    }
}

}
