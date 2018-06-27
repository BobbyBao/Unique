using System;
using System.Collections.Generic;
using System.Text;
using UniqueEngine;

namespace UniqueEditor.Samples
{
    [SampleDesc(sortOrder = 1)]
    public class CubeSample : Sample
    {
        public override void Enter()
        {
            base.Enter();

            New(out scene)
            .Component<Octree>()
            .Component<DebugRenderer>()
            .Child("Camera", c => c
                .Position(new Vector3(0.0f, 0.0f, -5.0f))
                .LookAt(new Vector3(0.0f, 0.0f, 0.0f))
                .Component<Camera>(cam => cam
                   .Store(out camera)
                )
            )

            .Child("Box", c => c
                .Position(Vector3.Zero)
                .Scaling(new Vector3(1.0f, 1.0f, 1.0f))
                .Component<StaticModel>(sm => sm
                    .Model("Cube")                    
                    .Material("Models/Box.material")
                )
            );

            viewport.Scene(scene).Camera(camera);
        }
    }
}
