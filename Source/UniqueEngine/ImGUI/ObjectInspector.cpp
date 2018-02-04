#include "UniquePCH.h"
#include "ObjectInspector.h"
#include "ImGUI.h"

namespace Unique
{

ObjectInspector::ObjectInspector() : Visitor(VS_READWRITE)
{
}


ObjectInspector::~ObjectInspector()
{
}

bool ObjectInspector::StartDocument(const String& fileName) { return true; }
void ObjectInspector::EndDocument() {}
bool ObjectInspector::StartObject(uint size) { return true; }
void ObjectInspector::EndObject() {}
SPtr<Object> ObjectInspector::CreateObject() { return nullptr; }
bool ObjectInspector::StartAttribute(const String& key) { return true; }
void ObjectInspector::EndAttribute() {}
bool ObjectInspector::StartArray(uint& size) { return true; }
void ObjectInspector::SetElement(uint index) {}
void ObjectInspector::EndArray() {}

void ObjectInspector::VisitBin(ByteArray& data) {}

void ObjectInspector::VisitPrimitive(std::string& data) {}

void ObjectInspector::VisitPrimitive(String& data) {}

void ObjectInspector::VisitPrimitive(bool& data) {}

void ObjectInspector::VisitPrimitive(char& data) {}

void ObjectInspector::VisitPrimitive(unsigned char& data) {}

void ObjectInspector::VisitPrimitive(short& data) {}

void ObjectInspector::VisitPrimitive(unsigned short& data) {}

void ObjectInspector::VisitPrimitive(int& data) 
{
}

void ObjectInspector::VisitPrimitive(unsigned int& data) {}

void ObjectInspector::VisitPrimitive(long long& data) {}

void ObjectInspector::VisitPrimitive(unsigned long long& data) {}

void ObjectInspector::VisitPrimitive(float& data) {}

void ObjectInspector::VisitPrimitive(double& data) {}

void ObjectInspector::VisitPrimitive(Vector2& data) {}

void ObjectInspector::VisitPrimitive(Vector3& data) {}

void ObjectInspector::VisitPrimitive(Vector4& data) {}

void ObjectInspector::VisitPrimitive(Color& data) {}

void ObjectInspector::VisitPrimitive(Quaternion& data) {}
}