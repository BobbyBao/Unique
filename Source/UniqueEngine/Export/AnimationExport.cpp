#include "UniquePCH.h"
#include "Animation/AnimatedModel.h"

using namespace Unique;


UNIQUE_C_API void AnimatedModel_SetModelAttr(AnimatedModel* self, ResourceRef* model)
{
	self->SetModelAttr(*model);
}
