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

	if (!CheckNeedSample()) {
		return; //!< これ以上のsampleは不必要
	}

	DeferredSurface surface;
	if (!surface.GetSurface(index)) {
		gIndirect[index] = float4(0.0f, 0.0f, 0.0f, 0.0f);
		return; // surfaceが存在しない
	}

	// primary trace.

	float4 diffuse_indirect  = float4(0.0f, 0.0f, 0.0f, 0.0f);
	float4 specular_indirect = float4(0.0f, 0.0f, 0.0f, 0.0f);
	
	for (uint i = 0; i < sampleStep; ++i) {

		uint currentSampleIndex = sampleStep * currentFrame + i;
		float2 xi = Hammersley(currentSampleIndex, sampleCount);

		static const float3 kMinFrenel = float3(0.04f, 0.04f, 0.04f); //!< 非金属の最小Frenel値

		//* cameraからの方向ベクトルを取得
		float3 v = normalize(gCamera.GetPosition() - surface.position);

		{ //!< Diffuseサンプリング

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

			diffuse_indirect.rgb += diffuseBRDF * payload.indirect.rgb * NdotL;
			diffuse_indirect.a   += payload.indirect.a > 0.0f ? 1.0f : 0.0f;
		}

		{ //!< Specularサンプル

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

			specular_indirect.rgb += specularBRDF * payload.indirect.rgb * NdotL;
			specular_indirect.a   += payload.indirect.a > 0.0f ? 1.0f : 0.0f;
		}
	}

	//diffuse_indirect.rgb  /= sampleCount; //!< 平均化
	//specular_indirect.rgb /= sampleCount;

	//float4 indirect = float4(0.0f, 0.0f, 0.0f, 0.0f);
	//indirect.rgb    = diffuse_indirect.rgb + specular_indirect.rgb;
	//indirect.a      = saturate(diffuse_indirect.a + specular_indirect.a);
	
	//gIndirect[index] += indirect;

	uint prev    = sampleStep * currentFrame;
	uint current = prev + sampleStep;

	diffuse_indirect.rgb  /= current;
	specular_indirect.rgb /= current;

	float4 indirect = gIndirect[index];
	indirect *= float(prev) / float(current);
	indirect.rgb += diffuse_indirect.rgb + specular_indirect.rgb;
	indirect.a    = saturate(indirect.a + diffuse_indirect.a + specular_indirect.a);
	
	gIndirect[index] = indirect;
}
