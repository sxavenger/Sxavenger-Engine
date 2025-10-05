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

	uint2 moment = gMoment[index];

	if (isResetMoment) {
		//!< 蓄積のリセット
		gReservoir[index] = float4(0.0f, 0.0f, 0.0f, 0.0f);
		gMoment[index]    = moment = uint2(0, Xorshift::xorshift32(index.x * index.y)); //!< moment.x : 現在のsample数, moment.y : xiのoffset
	}

	if (moment.x >= maxSampleCount) {
		return; //!< これ以上のsampleは不必要
	}

	if (moment.y == 0) {
		moment.y = Xorshift::xorshift32(index.x * index.y); //!< xiのoffsetを初期化
	}

	DeferredSurface surface;
	if (!surface.GetSurface(gDeferredBufferIndex.Get(), index)) {
		gReservoir[index] = float4(0.0f, 0.0f, 0.0f, 0.0f);
		return; // surfaceが存在しない
	}

	// primary trace.

	float4 diffuse_indirect  = float4(0.0f, 0.0f, 0.0f, 0.0f);
	float4 specular_indirect = float4(0.0f, 0.0f, 0.0f, 0.0f);

	for (uint i = 0; i < min(samplesPerFrame, maxSampleCount - moment.x); ++i) {

		uint currentSampleIndex   = moment.x + i;
		uint randamizeSampleIndex = (currentSampleIndex + moment.y) % maxSampleCount;
		//!< 各threadが異なるサンプルを取得するためのインデックス計算
		
		float2 xi = Hammersley(randamizeSampleIndex, maxSampleCount);

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

			float pdf = NdotL / kPi;

			if (pdf > 0.0f) {
				diffuse_indirect.rgb += diffuseBRDF * payload.indirect.rgb * kPi;
				diffuse_indirect.a   += payload.indirect.a > 0.0f ? 1.0f : 0.0f;
			}
		}

		// TODO: view依存をなくし, カメラ移動に対応する.

		//{ //!< Specularサンプル

		//	RayDesc desc;
		//	desc.Origin    = surface.position;
		//	desc.Direction = ImportanceSampleGGX(xi, surface.roughness, surface.normal);
		//	desc.TMin      = kTMin;
		//	desc.TMax      = kTMax;

		//	Payload payload = TracePrimaryRay(desc);

		//	//* 計算
		//	float3 h = normalize(v + desc.Direction);

		//	float NdotV = saturate(dot(surface.normal, v));
		//	float NdotL = saturate(dot(surface.normal, desc.Direction));
		//	float NdotH = saturate(dot(surface.normal, h));
		//	float VdotH = saturate(dot(v, h));

		//	float3 specularAlbedo = lerp(kMinFrenel, surface.albedo, surface.metallic);

		//	float3 f = F_SphericalGaussian(VdotH, specularAlbedo);
		//	float vh = V_HeightCorrelated(NdotV, NdotL, surface.roughness);
		//	float d  = D_GGX(NdotH, surface.roughness);

		//	float3 specularBRDF = SpecularBRDF(f, vh, d);

		//	float pdf = d * NdotH / (4.0f * VdotH + kEpsilon);

		//	if (pdf > 0.0f && NdotL > 0.0f) {
		//		specular_indirect.rgb += specularBRDF * payload.indirect.rgb * (NdotL / pdf);
		//		specular_indirect.a   += payload.indirect.a > 0.0f ? 1.0f : 0.0f;
		//	}
		//}
	}

	uint prev = moment.x;
	uint current = moment.x + min(samplesPerFrame, maxSampleCount - moment.x);

	float4 indirect = gReservoir[index] * float(prev);
	
	indirect.rgb += (diffuse_indirect.rgb + specular_indirect.rgb);
	indirect.rgb /= float(current);
	indirect.a = saturate(indirect.a + diffuse_indirect.a + specular_indirect.a);

	//float4 indirect = gReservoir[index];

	//indirect.rgb += (diffuse_indirect.rgb + specular_indirect.rgb) / float(maxSampleCount);
	//indirect.a    = 1.0f;
	
	gReservoir[index] = indirect;
	gMoment[index]    = uint2(current, moment.y);
	
}
