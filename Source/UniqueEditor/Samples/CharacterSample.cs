using System;
using System.Collections.Generic;
using System.Text;
using UniqueEngine;

namespace UniqueEditor.Samples
{
    [SampleDesc(sortOrder = 10)]
    public class CharacterSample : Sample
    {
        public override void Enter()
        {
            base.Enter();

            New(ref scene)
            .Component<Octree>()
            .Component<DebugRenderer>()
            .Child("Light", c => c
                .Direction(new Vector3(0.6f, -1.0f, 0.8f))
                .Component<Light>(light => light
                   .SetLightType(LightType.DIRECTIONAL)
                )

            )
            .Child("Camera", c => c
                .Position(new Vector3(0.0f, 10.0f, -30.0f))
                .LookAt(new Vector3(0.0f, 0.0f, 0.0f))
                .Component<Camera>(cam => cam
                   .Store(ref camera)
                )
            )

            .Child("Floor", c => c
                .Position(Vector3.Zero)
                .Scaling(new Vector3(100.0f, 100.0f, 100.0f))
                .Component<StaticModel>(sm => sm
                    .Model(new ResourceRef(TypeOf<Model>(), "Models/Plane.mdl"))
                    .Material(new ResourceRefList(TypeOf<Material>(), "Models/Stone.material"))
                )
            )

            .Child("Character", c => c
                .Rotation(new Quaternion(0, 180, 0))
                .Component<StaticModel>(sm => sm
                    .Model(new ResourceRef(TypeOf<Model>(), "Models/Kachujin/Kachujin.mdl"))
                    .Material(new ResourceRefList(TypeOf<Material>(), "Models/Kachujin/Materials/Kachujin.material"))
                )
            )            
            ;
    
            viewport.Scene(scene)
                .Camera(camera);
        }

        public override void Update(float timeStep)
        {
            base.Update(timeStep);
        }

        public override void Exit()
        {
            base.Exit();
        }
    }
}
