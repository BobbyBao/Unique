#include "UniquePCH.h"
#include "ClusteredLight.h"


ClusteredLight::ClusteredLight()
{
	/*
	uint32 lights[CLUSTER_Z][CLUSTER_Y][CLUSTER_X] = {};

	float3 scale = float3(float(CLUSTER_X), float(CLUSTER_Y), float(CLUSTER_Z)) / (m_AABBMax - m_AABBMin);
	float3 inv_scale = 1.0f / scale;

	for (uint i = 0; i < LIGHT_COUNT; i++)
	{
		const Light& light = m_Lights[i];

		const float3 p = (light.Position - m_AABBMin);
		const float3 p_min = (p - light.Radius) * scale;
		const float3 p_max = (p + light.Radius) * scale;

		// Cluster for the center of the light
		const int px = (int)floorf(p.x * scale.x);
		const int py = (int)floorf(p.y * scale.y);
		const int pz = (int)floorf(p.z * scale.z);

		// Cluster bounds for the light
		const int x0 = max((int)floorf(p_min.x), 0);
		const int x1 = min((int)ceilf(p_max.x), CLUSTER_X);
		const int y0 = max((int)floorf(p_min.y), 0);
		const int y1 = min((int)ceilf(p_max.y), CLUSTER_Y);
		const int z0 = max((int)floorf(p_min.z), 0);
		const int z1 = min((int)ceilf(p_max.z), CLUSTER_Z);

		const float radius_sqr = light.Radius * light.Radius;
		const uint32 mask = (1 << i);

		// Do AABB<->Sphere tests to figure out which clusters are actually intersected by the light
		for (int z = z0; z < z1; z++)
		{
			float dz = (pz == z) ? 0.0f : m_AABBMin.z + (pz < z ? z : z + 1) * inv_scale.z - light.Position.z;
			dz *= dz;

			for (int y = y0; y < y1; y++)
			{
				float dy = (py == y) ? 0.0f : m_AABBMin.y + (py < y ? y : y + 1) * inv_scale.y - light.Position.y;
				dy *= dy;
				dy += dz;

				for (int x = x0; x < x1; x++)
				{
					float dx = (px == x) ? 0.0f : m_AABBMin.x + (px < x ? x : x + 1) * inv_scale.x - light.Position.x;
					dx *= dx;
					dx += dy;

					if (dx < radius_sqr)
					{
						lights[z][y][x] |= mask;
					}
				}
			}
		}
	}

	// Upload the cluster data to a volume texture
	ID3D11Resource* tex = ((Direct3D11Renderer *)renderer)->getResource(m_Clusters);

	D3D11_MAPPED_SUBRESOURCE map;
	context->Map(tex, 0, D3D11_MAP_WRITE_DISCARD, 0, &map);
	for (uint z = 0; z < CLUSTER_Z; z++)
	{
		for (uint y = 0; y < CLUSTER_Y; y++)
		{
			memcpy(((uint8 *)map.pData) + z * map.DepthPitch + y * map.RowPitch, lights[z][y], CLUSTER_X * sizeof(uint32));
		}
	}
	context->Unmap(tex, 0);*/
}


ClusteredLight::~ClusteredLight()
{
}
