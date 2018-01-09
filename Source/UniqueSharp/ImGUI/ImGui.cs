using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using System.Security;
using System.Text;

namespace Unique.Engine
{
    public static partial class ImGUI
    {
        public static void Begin(string title, nk_rect bounds, nk_panel_flags flags)
        {
            nk_begin(ctx, title, bounds, (uint)flags);
        }

        public static void End()
        {
            nk_end(ctx);
        }

        static IntPtr ctx_;
        static IntPtr ctx
        {
            get
            {
                if(ctx_ == IntPtr.Zero)
                {
                    ctx_ = nk_ctx();
                }

                return ctx_;
            }

        }
    }
}
