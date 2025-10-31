//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "PathtracingCommon.hlsli"

//* lib
#include "../../Library/BRDF.hlsli"

//=========================================================================================
// local buffers
//=========================================================================================

////////////////////////////////////////////////////////////////////////////////////////////
// raygeneration main entry point
////////////////////////////////////////////////////////////////////////////////////////////
_RAYGENERATION void mainRaygeneration() {

	uint2 index     = DispatchRaysIndex().xy;
	uint2 dimension = DispatchRaysDimensions().xy;

	static const uint kDownSample = 2;
	static const uint kAtlas      = 16;
	// TODO: atlasのサイズを変更できるようにする

	uint2 surface_index = index * kDownSample;

	uint3 moment = gMoment[index].xyz;

	if (moment.x == 0) {
		moment.x = GetOffset(index);
	}

	DeferredSurface surface;
	if (!surface.GetSurface(gDeferredBufferIndex.Get(), surface_index)) {
		gReservoirDiffuse[index]  = float4(0.0f, 0.0f, 0.0f, 0.0f);
		gReservoirSpecular[index] = float4(0.0f, 0.0f, 0.0f, 0.0f);
		gMoment[index].xyz        = uint3(GetOffset(index), 0, 0);
		return; // surfaceが存在しない
	}

	static const float3 kMinFrenel = float3(0.04f, 0.04f, 0.04f); //!< 非金属の最小Frenel値

	//* cameraからの方向ベクトルを取得
	float3 v = normalize(gCamera.GetPosition() - surface.position);

	for (uint i = 0; i < samplesPerFrame; ++i) {

		{ //!< diffuse
			float3 diffuse_indirect = float3(0.0f, 0.0f, 0.0f);
		
			uint currentSampleIndex = (moment.y + i) % maxSampleCount;
			float2 xi = Hammersley(CalculateRandamizeSampleValue(currentSampleIndex, moment.x), maxSampleCount);
			//!< 各threadが異なるサンプルを取得するためのインデックス計算

			//!< Li(...)の取得
			RayDesc desc;
			desc.Origin    = surface.position;
			desc.Direction = ImportanceSampleLambert(xi, surface.normal);
			desc.TMin      = kTMin;
			desc.TMax      = kTMax;

			Payload payload = TracePrimaryRay(desc);

			//!< 計算
			float NdotL = saturate(dot(surface.normal, desc.Direction));

			float3 diffuseAlbedo = surface.albedo * (1.0f - kMinFrenel) * (1.0f - surface.metallic);
			float3 diffuseBRDF   = DiffuseBRDF(diffuseAlbedo);

			if (NdotL > 0.0f) {
				diffuse_indirect += diffuseBRDF * payload.indirect.rgb * kPi;
			}

			//!< atlasへの書き込み
			uint2 atlasIndex = index * kAtlas + uint2(currentSampleIndex / kAtlas, currentSampleIndex % kAtlas);

			float3 prev_diffuse_indirect = gAtlasDiffuse[atlasIndex].rgb;
			gAtlasDiffuse[atlasIndex]    = float4(diffuse_indirect, 1.0f);

			//!< reservoirの更新
			gReservoirDiffuse[index].rgb += (diffuse_indirect - prev_diffuse_indirect) / float(maxSampleCount);
			gReservoirDiffuse[index].a    = 1.0f;
		}

		{
			float3 specular_indirect = float3(0.0f, 0.0f, 0.0f);

			uint currentSampleIndex = (moment.z + i) % maxSampleCount;
			float2 xi = Hammersley(CalculateRandamizeSampleValue(currentSampleIndex, moment.x), maxSampleCount);
			//!< 各threadが異なるサンプルを取得するためのインデックス計算

			RayDesc desc;
			desc.Origin    = surface.position;
			desc.Direction = ImportanceSampleGGX(xi, surface.roughness, surface.normal);
			desc.TMin      = kTMin;
			desc.TMax      = kTMax;

			Payload payload = TracePrimaryRay(desc);

			//!< 計算
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

			float pdf = d * NdotH / max(4.0f * VdotH, kEpsilon);

			if (pdf > 0.0f && NdotL > 0.0f) {
				specular_indirect.rgb += specularBRDF * payload.indirect.rgb * (NdotL / pdf);
			}

			//!< atlasへの書き込み
			uint2 atlasIndex = index * kAtlas + uint2(currentSampleIndex / kAtlas, currentSampleIndex % kAtlas);

			float3 prev_specular_indirect = gAtlasSpecular[atlasIndex].rgb;
			gAtlasSpecular[atlasIndex]   = float4(specular_indirect, 1.0f);

			//!< reservoirの更新
			gReservoirSpecular[index].rgb += (specular_indirect - prev_specular_indirect) / float(maxSampleCount);
			gReservoirSpecular[index].a    = 1.0f;
		}

	}

	moment.y = (moment.y + samplesPerFrame) % maxSampleCount;
	moment.z = (moment.z + samplesPerFrame) % maxSampleCount;

	gMoment[index].xyz = moment;
}
