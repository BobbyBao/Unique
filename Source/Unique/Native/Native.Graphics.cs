using System;
using System.Collections.Generic;
using System.Security;
using System.Text;

namespace Unique.Engine
{
    [SuppressUnmanagedCodeSecurity]
    internal unsafe static partial class Native
    {

        /*
        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
        public static extern void unique_draw_geometry(byte view, GeometryInstance* geo);

        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
        public static extern void unique_draw_transient_geom(byte view, ref TransientVertexBuffer vertex_buffer,
        float* mtx, int vertex_start, int vertex_count, RenderState render_states, ShaderProgram shader_instance);

        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
        public static extern void unique_draw_transient_indexed(byte view, ref TransientVertexBuffer vertex_buffer, ref TransientIndexBuffer index_buffer,
        float* mtx, int index_start, int index_count, RenderState render_states, ShaderProgram shader_instance);

        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
        public static extern int unique_draw_geometries(PassData* view, IntPtr prim_list, uint num);
 */

    }
}
