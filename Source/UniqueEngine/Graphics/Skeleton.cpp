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

#include "Precompiled.h"

#include "../Graphics/Skeleton.h"
#include "../IO/Log.h"
#include "../io/Deserializer.h"
#include "../io/Serializer.h"
#include "../DebugNew.h"

namespace Unique
{

Skeleton::Skeleton() :
    rootBoneIndex_(M_MAX_UNSIGNED)
{
}

Skeleton::~Skeleton()
{
}

bool Skeleton::Load(Deserializer& source)
{
    ClearBones();

    if (source.IsEof())
        return false;

    unsigned bones = source.ReadUInt();
    bones_.reserve(bones);

    for (unsigned i = 0; i < bones; ++i)
    {
        Bone newBone;
        newBone.name_ = source.ReadString();
        newBone.nameHash_ = newBone.name_;
        newBone.parentIndex_ = source.ReadUInt();
        newBone.initialPosition_ = source.ReadVector3();
		newBone.initialRotation_ = source.ReadQuaternion();
        newBone.initialScale_ = source.ReadVector3();
        source.Read(&newBone.offsetMatrix_.m00_, sizeof(Matrix3x4));

        // Read bone collision data
        newBone.collisionMask_ = source.ReadUByte();
        if (newBone.collisionMask_ & BONECOLLISION_SPHERE)
            newBone.radius_ = source.ReadFloat();
        if (newBone.collisionMask_ & BONECOLLISION_BOX)
            newBone.boundingBox_ = source.ReadBoundingBox();

        if (newBone.parentIndex_ == i)
            rootBoneIndex_ = i;

        bones_.push_back(newBone);
    }

    return true;
}

bool Skeleton::Save(Serializer& dest) const
{
    if (!dest.WriteUInt((unsigned)bones_.size()))
        return false;

    for (unsigned i = 0; i < (unsigned)bones_.size(); ++i)
    {
        const Bone& bone = bones_[i];
        dest.WriteString(bone.name_);
        dest.WriteUInt(bone.parentIndex_);
        dest.WriteVector3(bone.initialPosition_);
        dest.WriteQuaternion(bone.initialRotation_);
        dest.WriteVector3(bone.initialScale_);
        dest.Write(bone.offsetMatrix_.Data(), sizeof(Matrix3x4));

        // Collision info
        dest.WriteUByte(bone.collisionMask_);
        if (bone.collisionMask_ & BONECOLLISION_SPHERE)
            dest.WriteFloat(bone.radius_);
        if (bone.collisionMask_ & BONECOLLISION_BOX)
            dest.WriteBoundingBox(bone.boundingBox_);
    }

    return true;
}

void Skeleton::Define(const Skeleton& src)
{
    ClearBones();

    bones_ = src.bones_;
    // Make sure we clear node references, if they exist
    // (AnimatedModel will create new nodes on its own)
    for (Vector<Bone>::iterator i = bones_.begin(); i != bones_.end(); ++i)
        i->node_.Reset();
    rootBoneIndex_ = src.rootBoneIndex_;
}

void Skeleton::SetRootBoneIndex(unsigned index)
{
    if (index < bones_.size())
        rootBoneIndex_ = index;
    else
        UNIQUE_LOGERROR("Root bone index out of bounds");
}

void Skeleton::ClearBones()
{
    bones_.clear();
    rootBoneIndex_ = M_MAX_UNSIGNED;
}

void Skeleton::Reset()
{
    for (Vector<Bone>::iterator i = bones_.begin(); i != bones_.end(); ++i)
    {
        if (i->animated_ && i->node_)
            i->node_->SetTransform(i->initialPosition_, i->initialRotation_, i->initialScale_);
    }
}

void Skeleton::ResetSilent()
{
    for (Vector<Bone>::iterator i = bones_.begin(); i != bones_.end(); ++i)
    {
        if (i->animated_ && i->node_)
            i->node_->SetTransformSilent(i->initialPosition_, i->initialRotation_, i->initialScale_);
    }
}


Bone* Skeleton::GetRootBone()
{
    return GetBone(rootBoneIndex_);
}

Bone* Skeleton::GetBone(unsigned index)
{
    return index < bones_.size() ? &bones_[index] : (Bone*)0;
}

Bone* Skeleton::GetBone(const String& name)
{
    return GetBone(StringID(name));
}

Bone* Skeleton::GetBone(const char* name)
{
    return GetBone(StringID(name));
}

Bone* Skeleton::GetBone(StringID nameHash)
{
    for (Vector<Bone>::iterator i = bones_.begin(); i != bones_.end(); ++i)
    {
        if (i->nameHash_ == nameHash)
            return &(*i);
    }

    return 0;
}

}
