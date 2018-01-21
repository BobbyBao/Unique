#include "UniquePCH.h"
#include "Graphics/Camera.h"
#include "Graphics/StaticModel.h"

using namespace Unique;

uExport(Camera, void, SetNearClip, float, nearClip)
uExport(Camera, void, SetFarClip, float, farClip)
uExport(Camera, void, SetFov, float, fov)
uExport(Camera, void, SetOrthoSize, const Vector2&, orthoSize)
uExport(Camera, void, SetAspectRatio, float, aspectRatio)
uExport(Camera, void, SetFillMode, FillMode, fillMode)
uExport(Camera, void, SetZoom, float, zoom)
uExport(Camera, void, SetLodBias, float, lodBias)
uExport(Camera, void, SetViewMask, unsigned, viewMask)
uExport(Camera, void, SetViewOverrideFlags, unsigned, flags)
uExport(Camera, void, SetOrthographic, bool)
uExport(Camera, void, SetAutoAspectRatio, bool)
uExport(Camera, void, SetProjectionOffset, const Vector2&)
uExport(Camera, void, SetUseReflection, bool)
uExport(Camera, void, SetReflectionPlane, const Plane&)
uExport(Camera, void, SetUseClipping, bool)
uExport(Camera, void, SetClipPlane, const Plane&)
uExport(Camera, void, SetFlipVertical, bool)

uExport(StaticModel, void, SetModel, Model*, model)
uExport(StaticModel, void, SetMaterial, Material*, material)
//uExport(StaticModel, void, SetMaterialAt, int, index, Material*, material)

UNIQUE_C_API void StaticModel_SetModelAttr(StaticModel* self, ResourceRef* model)
{
	self->SetModelAttr(*model);
}

UNIQUE_C_API void StaticModel_SetMaterialsAttr(StaticModel* self, ResourceRefList* material)
{
	self->SetMaterialsAttr(*material);
}

uExport(Graphics, float, GetWidth)
uExport(Graphics, int, GetHeight)

uExportStatic(Graphics, float, GetRenderWait)
uExportStatic(Graphics, float, GetUpdateWait)
