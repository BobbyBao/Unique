using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using System.Security;
using System.Text;

namespace Unique.Engine
{
    public struct BeginFrame
    {
        public int frameNumber;
        public float timeStep;
    }

    public struct Update
    {
        public float timeStep;
    }

    public struct PostUpdate
    {
        public float timeStep;
    }


    public struct RenderUpdate
    {
        public float timeStep;
    }

    public struct PostRenderUpdate
    {
        public float timeStep;
    }

    public struct GUI
    {
    }

    public struct EndFrame
    {
    }

    public struct LogMessage
    {
        public String.Data message;
        public int level;
    }

    
}
