using System;
using System.Collections.Generic;
using System.Text;

namespace Unique.Engine
{
    using static Native;

    public enum TransformSpace
    {
        LOCAL = 0,
        PARENT,
        WORLD
    }

    public partial class Node : Object
    {
        internal Node(IntPtr nativePtr) : base(nativePtr)
        {
        }

        public Node Position(Vector3 pos)
        {
            Node_SetPosition(native_, ref pos);
            return this;
        }

        public Node Rotation(Quaternion rot)
        {
            Node_SetRotation(native_, ref rot);
            return this;
        }

        public Node Scaling(Vector3 s)
        {
            Node_SetScale(native_, ref s);
            return this;
        }

        public Node Translate(Vector3 t, TransformSpace ts = TransformSpace.LOCAL)
        {
            Node_Translate(native_, ref t, ts);
            return this;
        }

        public Node Rotate(Quaternion r, TransformSpace ts = TransformSpace.LOCAL)
        {
            Node_Rotate(native_, ref r, ts);
            return this;
        }

        public Node Pitch(float p, TransformSpace ts = TransformSpace.LOCAL)
        {
            Node_Pitch(native_, p, ts);
            return this;
        }

        public Node Yaw(float p, TransformSpace ts = TransformSpace.LOCAL)
        {
            Node_Yaw(native_, p, ts);
            return this;
        }

        public Node Roll(float p, TransformSpace ts = TransformSpace.LOCAL)
        {
            Node_Roll(native_, p, ts);
            return this;
        }

        public Node LookAt(Vector3 target, TransformSpace ts = TransformSpace.LOCAL)
        {
            return LookAt(target, Vector3.Up, ts);
        }

        public Node LookAt(Vector3 target, Vector3 up, TransformSpace ts = TransformSpace.LOCAL)
        {
            Node_LookAt(native_, ref target, ref up, ts);
            return this;
        }

        public Node Scale(Vector3 s)
        {
            Node_Scale(native_, ref s);
            return this;
        }

        public Node Child(StringID name, bool temp = false)
        {
            return new Node(Node_CreateChild(native_, ref name, temp));
        }

        public Component Component(StringID type)
        {
            return PtrToObject(Node_GetOrCreateComponent(native_, ref type)) as Component;
        }

        public T Component<T>() where T : Component
        {
            return PtrToObject(Node_GetOrCreateComponent(native_, ref TypeOf<T>())) as T;
        }
    }
}
