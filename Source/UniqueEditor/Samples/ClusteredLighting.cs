﻿using System;
using System.Collections.Generic;
using System.Text;
using UniqueEngine;

namespace UniqueEditor.Samples
{
    [SampleDesc(sortOrder = 100)]
    public class ClusteredLighting : Sample
    {
        public override void Enter()
        {
            base.Enter();

            New(out scene)
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
                   .Store(out camera)
                )
            )

            .Child("Scene", c => c
                .Component<StaticModel>(sm => sm
                    .Model("Models/crytek-sponza/sponza.obj")
                )
            );

            viewport
                .Scene(scene)
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
