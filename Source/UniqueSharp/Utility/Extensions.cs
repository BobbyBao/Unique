using System;
using System.Collections.Generic;
using System.Text;

namespace UniqueEngine
{
    public static class Extensions
    {
        public static bool IsNullOrEmpty(this Array self)
        {
            return self == null || self.Length == 0;
        }
    }
}
