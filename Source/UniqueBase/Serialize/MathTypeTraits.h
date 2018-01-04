#pragma once
#include "TypeTraits.h"
#include "Math/Rect.h"
#include "Math/Sphere.h"
#include "Math/BoundingBox.h"
#include "Math/Plane.h"
#include "Math/Ray.h"

namespace Unique
{

	uClassTraits(Rect, "Min", self.min_, "Max", self.max_)
	uClassTraits(IntRect, "Left", self.left_, "Top", self.top_, "Right", self.right_, "Bottom", self.bottom_)
	uClassTraits(Sphere, "Center", self.center_, "Radius", self.radius_)
	uClassTraits(BoundingBox, "Min", self.min_, "Max", self.max_)
	uClassTraits(Plane, "Normal", self.normal_, "D", self.d_)
	uClassTraits(Ray, "Origin", self.origin_, "Direction", self.direction_)
}

