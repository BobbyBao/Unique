#include "UniquePCH.h"

using namespace Unique;


uExport(Component, unsigned, GetID)
uExport(Component, Node*, GetNode)
uExport(Component, Scene*, GetScene)
uExport(Component, bool, IsEnabled)

uExport(Node, void, SetPosition, const Vector3&)
uExport(Node, void, SetRotation, const Quaternion&)
uExport(Node, void, SetScale, const Vector3&)
uExport(Node, void, Translate, const Vector3&, pos, TransformSpace)
uExport(Node, void, Rotate, const Quaternion&, rot, TransformSpace)
uExport(Node, void, Pitch, float, pitch, TransformSpace)
uExport(Node, void, Yaw, float, yaw, TransformSpace)
uExport(Node, void, Roll, float, roll, TransformSpace)
uExport(Node, void, LookAt, const Vector3&, target, const Vector3&, up, TransformSpace)
uExport(Node, void, Scale, const Vector3&)

UNIQUE_C_API Node* Node_CreateChild(Node* self, const StringID& name, bool temp)
{
	return self->CreateChild(name, 0, temp);
}

UNIQUE_C_API Node* Node_GetChild(Node* self, const StringID& name, bool recursive)
{
	return self->GetChild(name, recursive);
}

UNIQUE_C_API int Node_GetChildNum(Node* self, bool recursive)
{
	return self->GetNumChildren(recursive);
}

UNIQUE_C_API Node* Node_GetChildAt(Node* self, int index)
{
	return self->GetChild(index);
}

UNIQUE_C_API Component* Node_CreateComponent(Node* self, const StringID& type)
{
	return self->CreateComponent(type);
}

UNIQUE_C_API Component* Node_GetOrCreateComponent(Node* self, const StringID& type)
{
	return self->GetOrCreateComponent(type);
}

UNIQUE_C_API Component* Node_GetComponent(Node* self, const StringID& type, bool recursive)
{
	return self->GetComponent(type, recursive);
}

UNIQUE_C_API Component* Node_GetParentComponent(Node* self, const StringID& type, bool fullTraversal)
{
	return self->GetParentComponent(type, fullTraversal);
}