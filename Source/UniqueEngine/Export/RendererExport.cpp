#include "UniquePCH.h"

using namespace Unique;


UNIQUE_C_API Viewport* Renderer_GetViewport(Renderer* self, int index)
{
	return self->GetViewport(index);
}

UNIQUE_C_API void Renderer_SetViewport(Renderer* self, int index, Viewport* vp)
{
	self->SetViewport(index, vp);
}

UNIQUE_C_API void Viewport_SetRect(Viewport* self, const IntRect& rect)
{
	self->SetRect(rect);
}

UNIQUE_C_API void Viewport_SetScene(Viewport* self, Scene* scene)
{
	self->SetScene(scene);
}

UNIQUE_C_API void Viewport_SetCamera(Viewport* self, Camera* camera)
{
	self->SetCamera(camera);
}

UNIQUE_C_API void Viewport_SetDebug(Viewport* self, byte debug)
{
	self->SetDrawDebug(debug != 0);
}