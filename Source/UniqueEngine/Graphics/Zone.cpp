//
// Copyright (c) 2008-2017 the Unique project.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//

#include "UniquePCH.h"

#include "Core/Context.h"
#include "../Graphics/DebugRenderer.h"
#include "../Graphics/Octree.h"
//#include "../Graphics/TextureCube.h"
#include "../Graphics/Zone.h"
#include "Resource/ResourceCache.h"
#include "../Scene/Node.h"
#include "../Scene/Scene.h"

#include "DebugNew.h"

namespace Unique
{

static const Vector3 DEFAULT_BOUNDING_BOX_MIN(-10.0f, -10.0f, -10.0f);
static const Vector3 DEFAULT_BOUNDING_BOX_MAX(10.0f, 10.0f, 10.0f);
static const Color DEFAULT_AMBIENT_COLOR(0.25f, 0.25f, 0.25f);
static const Color DEFAULT_FOG_COLOR(0.5f, 0.5f, 0.5f);
static const float DEFAULT_FOG_START = 250.0f;
static const float DEFAULT_FOG_END = 1000.0f;
static const float DEFAULT_FOG_HEIGHT = 0.0f;
static const float DEFAULT_FOG_HEIGHT_SCALE = 0.5f;

extern const char* SCENE_CATEGORY;

uObject(Zone)
{
	uFactory(SCENE_CATEGORY);
	
    uAccessor("Is Enabled", IsEnabled, SetEnabled, true);
    uAttribute("Bounding Box Min", boundingBox_.min_, DEFAULT_BOUNDING_BOX_MIN);
    uAttribute("Bounding Box Max", boundingBox_.max_, DEFAULT_BOUNDING_BOX_MAX);
    uAttribute("Ambient Color", ambientColor_, DEFAULT_AMBIENT_COLOR);
    uAttribute("Fog Color", fogColor_, DEFAULT_FOG_COLOR);
    uAttribute("Fog Start", fogStart_, DEFAULT_FOG_START);
    uAttribute("Fog End", fogEnd_, DEFAULT_FOG_END);
    uAttribute("Fog Height", fogHeight_, DEFAULT_FOG_HEIGHT);
    uAttribute("Fog Height Scale", fogHeightScale_, DEFAULT_FOG_HEIGHT_SCALE);
    uAttribute("Height Fog Mode", heightFog_, false);
    uAttribute("Override Mode", override_, false);
    uAttribute("Ambient Gradient", ambientGradient_, false);
    uAttribute("Priority", priority_, 0);
    uMixedAccessor("Zone Texture", GetZoneTextureAttr, SetZoneTextureAttr, ResourceRef(Texture::GetTypeStatic()));
    uAttribute("Light Mask", lightMask_, DEFAULT_LIGHTMASK);
    uAttribute("Shadow Mask", shadowMask_, DEFAULT_SHADOWMASK);
    uAccessor("Zone Mask", GetZoneMask, SetZoneMask, DEFAULT_ZONEMASK);
}

Zone::Zone() :
    Drawable(DRAWABLE_ZONE),
    inverseWorldDirty_(true),
    heightFog_(false),
    override_(false),
    ambientGradient_(false),
    ambientColor_(DEFAULT_AMBIENT_COLOR),
    fogColor_(DEFAULT_FOG_COLOR),
    fogStart_(DEFAULT_FOG_START),
    fogEnd_(DEFAULT_FOG_END),
    fogHeight_(DEFAULT_FOG_HEIGHT),
    fogHeightScale_(DEFAULT_FOG_HEIGHT_SCALE),
    priority_(0)
{
    boundingBox_ = BoundingBox(DEFAULT_BOUNDING_BOX_MIN, DEFAULT_BOUNDING_BOX_MAX);
}

Zone::~Zone()
{
}
/*
void Zone::RegisterObject(Context* context)
{
    context->RegisterFactory<Zone>(SCENE_CATEGORY);

}*/

void Zone::DrawDebugGeometry(DebugRenderer* debug, bool depthTest)
{
    if (debug && IsEnabledEffective())
        debug->AddBoundingBox(boundingBox_, node_->GetWorldTransform(), Color::GREEN, depthTest);
}

void Zone::SetBoundingBox(const BoundingBox& box)
{
    boundingBox_ = box;
    OnMarkedDirty(node_);
}

void Zone::SetAmbientColor(const Color& color)
{
    ambientColor_ = color;
}

void Zone::SetFogColor(const Color& color)
{
    fogColor_ = color;
}

void Zone::SetFogStart(float start)
{
    if (start < 0.0f)
        start = 0.0f;

    fogStart_ = start;
}

void Zone::SetFogEnd(float end)
{
    if (end < 0.0f)
        end = 0.0f;

    fogEnd_ = end;
}

void Zone::SetFogHeight(float height)
{
    fogHeight_ = height;
}

void Zone::SetFogHeightScale(float scale)
{
    fogHeightScale_ = scale;
}

void Zone::SetPriority(int priority)
{
    priority_ = priority;
}

void Zone::SetZoneTexture(Texture* texture)
{
    zoneTexture_ = texture;
}

void Zone::SetHeightFog(bool enable)
{
    heightFog_ = enable;
}

void Zone::SetOverride(bool enable)
{
    override_ = enable;
}

void Zone::SetAmbientGradient(bool enable)
{
    ambientGradient_ = enable;
}

const Matrix3x4& Zone::GetInverseWorldTransform() const
{
    if (inverseWorldDirty_)
    {
        inverseWorld_ = node_ ? node_->GetWorldTransform().Inverse() : Matrix3x4::IDENTITY;
        inverseWorldDirty_ = false;
    }

    return inverseWorld_;
}

const Color& Zone::GetAmbientStartColor()
{
    if (!ambientGradient_)
        return ambientColor_;

    if (!lastAmbientStartZone_ || !lastAmbientEndZone_)
        UpdateAmbientGradient();

    return ambientStartColor_;
}

const Color& Zone::GetAmbientEndColor()
{
    if (!ambientGradient_)
        return ambientColor_;

    if (!lastAmbientStartZone_ || !lastAmbientEndZone_)
        UpdateAmbientGradient();

    return ambientEndColor_;
}

bool Zone::IsInside(const Vector3& point) const
{
    // Use an oriented bounding box test
    Vector3 localPoint(GetInverseWorldTransform() * point);
    return boundingBox_.IsInside(localPoint) != OUTSIDE;
}

void Zone::SetZoneTextureAttr(const ResourceRef& value)
{
    ResourceCache& cache = GetSubsystem<ResourceCache>();
    zoneTexture_ = static_cast<Texture*>(cache.GetResource(value.type_, value.name_));
}

ResourceRef Zone::GetZoneTextureAttr() const
{
    return GetResourceRef(zoneTexture_, Texture::GetTypeStatic());
}

void Zone::OnMarkedDirty(Node* node)
{
    // Due to the octree query and weak pointer manipulation, is not safe from worker threads
    Scene* scene = GetScene();
    if (scene && scene->IsThreadedUpdate())
    {
        scene->DelayedMarkedDirty(this);
        return;
    }

    Drawable::OnMarkedDirty(node);

    // Clear zone reference from all drawables inside the bounding box, and mark gradient dirty in neighbor zones
    ClearDrawablesZone();

    inverseWorldDirty_ = true;
}

void Zone::OnWorldBoundingBoxUpdate()
{
    worldBoundingBox_ = boundingBox_.Transformed(node_->GetWorldTransform());
}

void Zone::UpdateAmbientGradient()
{
    // In case no neighbor zones are found, reset ambient start/end with own ambient color
    ambientStartColor_ = ambientColor_;
    ambientEndColor_ = ambientColor_;
    lastAmbientStartZone_ = this;
    lastAmbientEndZone_ = this;

    if (octant_)
    {
        const Matrix3x4& worldTransform = node_->GetWorldTransform();
        Vector3 center = boundingBox_.Center();
        Vector3 minZPosition = worldTransform * Vector3(center.x_, center.y_, boundingBox_.min_.z_);
        Vector3 maxZPosition = worldTransform * Vector3(center.x_, center.y_, boundingBox_.max_.z_);

        PODVector<Zone*> result;
        {
            PointOctreeQuery query(reinterpret_cast<PODVector<Drawable*>&>(result), minZPosition, DRAWABLE_ZONE);
            octant_->GetRoot()->GetDrawables(query);
        }

        // Gradient start position: get the highest priority zone that is not this zone
        int bestPriority = M_MIN_INT;
        Zone* bestZone = nullptr;
        for (auto i = result.begin(); i != result.end(); ++i)
        {
            Zone* zone = *i;
            int priority = zone->GetPriority();
            if (priority > bestPriority && zone != this && zone->IsInside(minZPosition))
            {
                bestZone = zone;
                bestPriority = priority;
            }
        }

        if (bestZone)
        {
            ambientStartColor_ = bestZone->GetAmbientColor();
            lastAmbientStartZone_ = bestZone;
        }

        // Do the same for gradient end position
        {
            PointOctreeQuery query(reinterpret_cast<PODVector<Drawable*>&>(result), maxZPosition, DRAWABLE_ZONE);
            octant_->GetRoot()->GetDrawables(query);
        }
        bestPriority = M_MIN_INT;
        bestZone = nullptr;

        for (auto i = result.begin(); i != result.end(); ++i)
        {
            Zone* zone = *i;
            int priority = zone->GetPriority();
            if (priority > bestPriority && zone != this && zone->IsInside(maxZPosition))
            {
                bestZone = zone;
                bestPriority = priority;
            }
        }

        if (bestZone)
        {
            ambientEndColor_ = bestZone->GetAmbientColor();
            lastAmbientEndZone_ = bestZone;
        }
    }
}

void Zone::OnRemoveFromOctree()
{
    ClearDrawablesZone();
}

void Zone::ClearDrawablesZone()
{
    if (octant_ && lastWorldBoundingBox_.Defined())
    {
        PODVector<Drawable*> result;
        BoxOctreeQuery query(result, lastWorldBoundingBox_, DRAWABLE_GEOMETRY | DRAWABLE_ZONE);
        octant_->GetRoot()->GetDrawables(query);

        for (auto i = result.begin(); i != result.end(); ++i)
        {
            Drawable* drawable = *i;
            unsigned drawableFlags = drawable->GetDrawableFlags();
            if (drawableFlags & DRAWABLE_GEOMETRY)
                drawable->SetZone(nullptr);
            else if (drawableFlags & DRAWABLE_ZONE)
            {
                Zone* zone = static_cast<Zone*>(drawable);
                zone->lastAmbientStartZone_.Reset();
                zone->lastAmbientEndZone_.Reset();
            }
        }
    }

    lastWorldBoundingBox_ = GetWorldBoundingBox();
    lastAmbientStartZone_.Reset();
    lastAmbientEndZone_.Reset();
}

}
