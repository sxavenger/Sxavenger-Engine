//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "LightRender.hlsli"

//* component
#include "../../Component/DirectionalLightComponent.hlsli"

//=========================================================================================
// buffers
//=========================================================================================

StructuredBuffer<DirectionalLightComponent> gParameters : register(t0);

////////////////////////////////////////////////////////////////////////////////////////////
// main
////////////////////////////////////////////////////////////////////////////////////////////
[earlydepthstencil]
PSOutput main(PSInput input) {

	PSOutput output = (PSOutput)0;

	//* Deferred Pass情報の取得
	GBuffer::Surface surface;
	surface.FetchSurface(GBuffer::FetchArgument::Create(input.position.xy, dimension, gCamera.projInv, gCamera.world));

	//* Lightの情報を取得
	float3 l = gParameters[input.instanceId].GetDirectionFromSurface(gTransforms[input.instanceId].GetForwardDirection()); //!< lightの方向ベクトル

	//* cameraからの方向ベクトルを取得
	float3 v = normalize(gCamera.GetPosition() - surface.position); //!< cameraからの方向ベクトルを取得

	BxDFAlbedo albedo   = BxDFAlbedo::Create(surface.albedo, surface.metallic);
	BxDFContext context = BxDFContext::Create(surface.normal, v, l);

	if (!context.IsValid()) {
		discard;
	}

	//* Lightの影響範囲
	float3 color_mask = gParameters[input.instanceId].GetColorMask();
	float light_mask  = gParameters[input.instanceId].GetLightMask(gScene, gTransforms[input.instanceId].GetForwardDirection(), surface.position);

	output.color.rgb = EvaluateBRDF(albedo, context, surface.roughness) * context.NdotL * color_mask * light_mask * surface.ao;
	// todo: specularFactorを追加

	output.color.a = 1.0f;
	return output;
	
}
