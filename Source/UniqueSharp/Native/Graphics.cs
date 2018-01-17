using System;
using System.Collections.Generic;
using System.Security;
using System.Text;

namespace Unique.Engine
{
    /// Device type
    public enum DeviceType : Int32
    {
        Undefined = 0,  ///< Undefined device
        D3D11,      ///< D3D11 device
        D3D12,      ///< D3D12 device
        OpenGL,     ///< OpenGL device 
        OpenGLES    ///< OpenGLES device
    }

    [SuppressUnmanagedCodeSecurity]
    public partial class Graphics
    {

    }
}
