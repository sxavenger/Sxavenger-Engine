#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "ProbeCommon.hlsli"

//* library
#include "../../Library/Math.hlsli"

//* component
#include "../../Component/CameraComponent.hlsli"

//=========================================================================================
// buffers
//=========================================================================================

ConstantBuffer<CameraComponent> gCamera : register(b0);

ConstantBuffer<ProbeConfig> Config : register(b1);

Texture2DArray<float4> gProbeIrradiance : register(t0);
SamplerState gSampler : register(s0);

////////////////////////////////////////////////////////////////////////////////////////////
// [Input / Output] structure
////////////////////////////////////////////////////////////////////////////////////////////

struct VSOutput {

	//=========================================================================================
	// public variables
	//=========================================================================================

	float4 position  : SV_Position;
	float3 normal    : NORMAL;
	uint3 probeIndex : PROBE_INDEX;

	//=========================================================================================
	// public methods
	//=========================================================================================

	void CalculateProbeVertex(uint vertexId, uint instanceId) {

		//* probe index calculation
		{

			uint xyCount = Config.probeCount.x * Config.probeCount.y;
			
			uint z = instanceId / (Config.probeCount.x * Config.probeCount.y);
			uint r = instanceId % (Config.probeCount.x * Config.probeCount.y);

			uint y = r / Config.probeCount.x;
			uint x = r % Config.probeCount.x;

			probeIndex = uint3(x, y, z);
		}

		//* vertex calculation
		{
			const uint kLatitude  = 8; //!< 緯度分割
			const uint kLongitude = 16; //!< 経度分割
			// vertex count = kLatitude * kLongitude * 6

			const float kRadius = 0.8f;

			uint vertex_id  = vertexId % 6;
			uint quad_index = vertexId / 6;

			uint lon = quad_index % kLongitude;
			uint lat = quad_index / kLongitude;

			float u[2] = { float(lon) / kLongitude, float(lon + 1) / kLongitude };
			float v[2] = { float(lat) / kLatitude,  float(lat + 1) / kLatitude };
			
			float theta[2] = { u[0] * kTau, u[1] * kTau };
			float phi[2]   = { v[0] * kPi,  v[1] * kPi };

			float2 angle;

			switch (vertex_id) {
				case 0: angle.x = theta[0]; angle.y = phi[0]; break;
				case 1: angle.x = theta[1]; angle.y = phi[0]; break;
				case 2: angle.x = theta[0]; angle.y = phi[1]; break;

				case 3: angle.x = theta[1]; angle.y = phi[0]; break;
				case 4: angle.x = theta[1]; angle.y = phi[1]; break;
				case 5: angle.x = theta[0]; angle.y = phi[1]; break;
			}

			float3 direction;
			direction.x = sin(angle.y) * cos(angle.x);
			direction.y = cos(angle.y);
			direction.z = sin(angle.y) * sin(angle.x);

			float3 local = direction * kRadius;
			float3 world = GetProbePosition(probeIndex, Config, float3(1, 1, 1)) + local;

			position = mul(float4(world, 1.0f), gCamera.GetViewProj());
			normal   = normalize(direction);
		}
	
	}
	
};

struct PSOutput {

	//=========================================================================================
	// public variables
	//=========================================================================================
	
	float4 color : SV_Target;

	//=========================================================================================
	// public methods
	//=========================================================================================

	void SetColor(uint3 probeIndex, float3 direction) {
		float2 uv = EncodeOctahedral(normalize(direction));
		float2 section = 1.0f / float2(Config.probeCount.xy);

		float2 probe_uv = (probeIndex.xy + uv) * section;

		color.rgb = gProbeIrradiance.SampleLevel(gSampler, float3(probe_uv, probeIndex.z), 0).rgb;
		color.a   = 1.0f;

	}
	
};
