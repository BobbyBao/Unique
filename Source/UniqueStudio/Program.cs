using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Unique.Engine;

namespace UniqueStudio
{
    class Program
    {
        static void Main(string[] args)
        {
            Application app = new Application(new string[] {"Test"});
            app.Run();
        }
    }
}
