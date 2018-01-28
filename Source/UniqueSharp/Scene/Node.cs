using System;
using System.Collections.Generic;
using System.Text;

namespace UniqueEngine
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
        public Node()
        {
        }

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

        public Node Scaling(float s)
        {
            return Scaling(new Vector3(s, s, s));
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

        public Node Child(StringID name, Action<Node> action = null)
        {
            Node node = new Node(Node_CreateChild(native_, ref name, false));
            if(action != null)
            {
                action(node);
            }
            return this;
        }

        public Node TempChild(StringID name, Action<Node> action = null)
        {
            Node node = new Node(Node_CreateChild(native_, ref name, true));
            if(action != null)
            {
                action(node);
            }
            return this;
        }

        public Node GetChild(StringID name, bool recursive = false)
        {
            Node node = PtrToObject(Node_GetChild(native_, ref name, recursive)) as Node;
            return node;
        }

        public Node Component(StringID type, Action<Component> action = null)
        {
            Component c = PtrToObject(Node_GetOrCreateComponent(native_, ref type)) as Component;
            action?.Invoke(c);
            return this;
        }

        public Node Component<T>(Action<T> action = null) where T : Component
        {
            return Component(TypeOf<T>(), (c)=> 
            {
                if(c != null)
                {
                    action?.Invoke(c as T);
                }
            });
        }

        public T GetComponent<T>(bool recursive = false) where T : Component
        {
            return PtrToObject(Node_GetComponent(native_, ref TypeOf<T>(), recursive)) as T;
        }

        public Component GetComponent(StringID type, bool recursive = false)
        {
            return PtrToObject(Node_GetComponent(native_, ref type, recursive)) as Component;
        }
    }
}
