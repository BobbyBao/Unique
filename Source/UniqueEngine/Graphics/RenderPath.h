//
// Copyright (c) 2008-2017 the Urho3D project.
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

#pragma once

#include "Container/Ptr.h"
#include "Container/RefCounted.h"
#include "Graphics/GraphicsDefs.h"
#include "Core/Variant.h"
#include "Math/Color.h"
#include "Math/Vector4.h"
#include "RenderPass.h"

namespace Unique
{
/// Rendertarget size mode.
enum RenderTargetSizeMode
{
    SIZE_ABSOLUTE = 0,
    SIZE_VIEWPORTDIVISOR,
    SIZE_VIEWPORTMULTIPLIER
};

/// Rendertarget definition.
struct UNIQUE_API RenderTargetInfo
{
    /// Construct.
    RenderTargetInfo() :
        size_(Vector2::ZERO),
        sizeMode_(SIZE_ABSOLUTE),
        multiSample_(1),
        autoResolve_(true),
        enabled_(true),
        cubemap_(false),
        filtered_(false),
        sRGB_(false),
        persistent_(false)
    {
    }
	
    /// Name.
    String name_;
    /// Tag name.
    String tag_;
    /// Texture format.
    unsigned format_;
    /// Absolute size or multiplier.
    Vector2 size_;
    /// Size mode.
    RenderTargetSizeMode sizeMode_;
    /// Multisampling level (1 = no multisampling).
    int multiSample_;
    /// Multisampling autoresolve flag.
    bool autoResolve_;
    /// Enabled flag.
    bool enabled_;
    /// Cube map flag.
    bool cubemap_;
    /// Filtering flag.
    bool filtered_;
    /// sRGB sampling/writing mode flag.
    bool sRGB_;
    /// Should be persistent and not shared/reused between other buffers of same size.
    bool persistent_;
};

/// Rendering path definition. A sequence of commands (e.g. clear screen, draw objects with specific pass) that yields the scene rendering result.
class UNIQUE_API RenderPath : public Object
{
	uRTTI(RenderPath, Object)
public:
    /// Construct.
    RenderPath();
    /// Destruct.
    virtual ~RenderPath() override;

    /// Clone the rendering path.
    SPtr<RenderPath> Clone();

    /// Enable/disable commands and rendertargets by tag.
    void SetEnabled(const String& tag, bool active);
    /// Return true of any of render targets or commands with specified tag are enabled.
    bool IsEnabled(const String& tag) const;
    /// Return true if renderpath or command with given tag exists.
    bool IsAdded(const String& tag) const;
    /// Toggle enabled state of commands and rendertargets by tag.
    void ToggleEnabled(const String& tag);
    /// Assign rendertarget at index.
    void SetRenderTarget(unsigned index, const RenderTargetInfo& info);
    /// Add a rendertarget.
    void AddRenderTarget(const RenderTargetInfo& info);
    /// Remove a rendertarget by index.
    void RemoveRenderTarget(unsigned index);
    /// Remove a rendertarget by name.
    void RemoveRenderTarget(const String& name);
    /// Remove rendertargets by tag name.
    void RemoveRenderTargets(const String& tag);
    /// Assign command at index.
    void SetCommand(unsigned index, RenderPass* command);
    /// Add a command to the end of the list.
    void AddCommand(RenderPass* command);
    /// Insert a command at a position.
    void InsertCommand(unsigned index, RenderPass* command);
    /// Remove a command by index.
    void RemoveCommand(unsigned index);
    /// Remove commands by tag name.
    void RemoveCommands(const String& tag);

    /// Return number of rendertargets.
    unsigned GetNumRenderTargets() const { return (uint)renderTargets_.size(); }

    /// Return number of commands.
    unsigned GetNumCommands() const { return (uint)commands_.size(); }

    /// Return command at index, or null if does not exist.
    RenderPass* GetCommand(unsigned index) { return index < commands_.size() ? commands_[index] : nullptr; }

    /// Rendertargets.
    Vector<RenderTargetInfo> renderTargets_;
    /// Rendering commands.
    Vector<SPtr<RenderPass>> commands_;

};

}
