//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "PathtracingCommon.hlsli"

//* lib
#include "../../Library/BRDF.hlsli"
#include "../../Library/Random.hlsli"

//=========================================================================================
// local buffers
//=========================================================================================

////////////////////////////////////////////////////////////////////////////////////////////
// raygeneration main entry point
////////////////////////////////////////////////////////////////////////////////////////////
_RAYGENERATION void mainRaygeneration() {

	uint2 index     = DispatchRaysIndex().xy;
	uint2 dimension = DispatchRaysDimensions().xy;

	uint type = DispatchRaysDimensions().z;
	// todo: diffuseとspecularで2thread構成にする.

	uint3 moment = gMoment[index].xyz;

	if (isResetMoment) {
		//!< 蓄積のリセット
		gReservoirDiffuse[index]  = float4(0.0f, 0.0f, 0.0f, 0.0f);
		gReservoirSpecular[index] = float4(0.0f, 0.0f, 0.0f, 0.0f);
		gMoment[index].xyz        = moment = uint3(Xorshift::xorshift32(index.x * index.y), 0, 0);
		// x: Hammerselyのoffset y: sample数(diffuse) z: sample数(specular, 継承不可)
	}

	if (moment.x == 0) {
		moment.x = Xorshift::xorshift32(index.x * index.y); //!< xiのoffsetを初期化
	}

	DeferredSurface surface;
	if (!surface.GetSurface(gDeferredBufferIndex.Get(), index)) {
		gReservoirDiffuse[index]  = float4(0.0f, 0.0f, 0.0f, 0.0f);
		gReservoirSpecular[index] = float4(0.0f, 0.0f, 0.0f, 0.0f);
		gMoment[index].xyz        = moment = uint3(Xorshift::xorshift32(index.x * index.y), 0, 0);
		return; // surfaceが存在しない
	}

	static const float3 kMinFrenel = float3(0.04f, 0.04f, 0.04f); //!< 非金属の最小Frenel値

	//* cameraからの方向ベクトルを取得
	float3 v = normalize(gCamera.GetPosition() - surface.position);

	if (moment.y < maxSampleCount) { //!< diffuse primary trace

		float4 diffuse_indirect = float4(0.0f, 0.0f, 0.0f, 0.0f);

		for (uint i = 0; i < min(samplesPerFrame, maxSampleCount - moment.y); ++i) {

			uint currentSampleIndex   = moment.y + i;
			uint randamizeSampleIndex = (currentSampleIndex + moment.x) % maxSampleCount;
			//!< 各threadが異なるサンプルを取得するためのインデックス計算
			
			float2 xi = Hammersley(GetSampleIndex(randamizeSampleIndex), maxSampleCount);

			RayDesc desc;
			desc.Origin    = surface.position;
			desc.Direction = ImportanceSampleLambert(xi, surface.normal);
			desc.TMin      = kTMin;
			desc.TMax      = kTMax;

			Payload payload = TracePrimaryRay(desc);
			// Li(...) = payload.indirect;

			//* 計算
			float NdotL = saturate(dot(surface.normal, desc.Direction));

			float3 diffuseAlbedo = surface.albedo * (1.0f - kMinFrenel) * (1.0f - surface.metallic);
			float3 diffuseBRDF   = DiffuseBRDF(diffuseAlbedo);

			float pdf = NdotL / kPi;

			if (pdf > 0.0f) {
				diffuse_indirect.rgb += diffuseBRDF * payload.indirect.rgb * kPi;
				diffuse_indirect.a   += payload.indirect.a > 0.0f ? 1.0f : 0.0f;
			}
		}

		uint prev    = moment.y;
		uint current = moment.y + min(samplesPerFrame, maxSampleCount - moment.y);

		float4 reservoir_diffuse = gReservoirDiffuse[index] * float(prev) / float(current);
		reservoir_diffuse.rgb   += diffuse_indirect.rgb / float(current);
		reservoir_diffuse.a      = 1.0f;

		gReservoirDiffuse[index] = reservoir_diffuse;
		
		moment.y = current;
	}

	if (moment.z < maxSampleCount) { //!< specular primary trace

		float4 specular_indirect = float4(0.0f, 0.0f, 0.0f, 0.0f);

		for (uint j = 0; j < min(samplesPerFrame, maxSampleCount - moment.z); ++j) {

			uint currentSampleIndex   = moment.z + j;
			uint randamizeSampleIndex = (currentSampleIndex + moment.x) % maxSampleCount;
			//!< 各threadが異なるサンプルを取得するためのインデックス計算
			
			float2 xi = Hammersley(GetSampleIndex(randamizeSampleIndex), maxSampleCount);

			RayDesc desc;
			desc.Origin    = surface.position;
			desc.Direction = ImportanceSampleGGX(xi, surface.roughness, surface.normal);
			desc.TMin      = kTMin;
			desc.TMax      = kTMax;

			Payload payload = TracePrimaryRay(desc);

			//* 計算
			float3 h = normalize(v + desc.Direction);

			float NdotV = saturate(dot(surface.normal, v));
			float NdotL = saturate(dot(surface.normal, desc.Direction));
			float NdotH = saturate(dot(surface.normal, h));
			float VdotH = saturate(dot(v, h));

			float3 specularAlbedo = lerp(kMinFrenel, surface.albedo, surface.metallic);

			float3 f = F_SphericalGaussian(VdotH, specularAlbedo);
			float vh = V_HeightCorrelated(NdotV, NdotL, surface.roughness);
			float d  = D_GGX(NdotH, surface.roughness);

			float3 specularBRDF = SpecularBRDF(f, vh, d);

			float pdf = d * NdotH / (4.0f * VdotH + kEpsilon);

			if (pdf > 0.0f && NdotL > 0.0f) {
				specular_indirect.rgb += specularBRDF * payload.indirect.rgb * (NdotL / pdf);
				specular_indirect.a   += payload.indirect.a > 0.0f ? 1.0f : 0.0f;
			}
		}

		uint prev    = moment.z;
		uint current = moment.z + min(samplesPerFrame, maxSampleCount - moment.z);

		float4 reservoir_specular = gReservoirSpecular[index];
		reservoir_specular.rgb   += specular_indirect.rgb / float(current);
		reservoir_specular.a      = 1.0f;

		gReservoirSpecular[index] = reservoir_specular;

		moment.z = current;
	}
	
	gMoment[index].xyz = moment;
	
}
