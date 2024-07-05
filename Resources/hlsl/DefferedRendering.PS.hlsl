//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "DefferedRendering.hlsli"
#include "Lighting.hlsli"
#include "camera.hlsli"

//=========================================================================================
// Output
//=========================================================================================
struct DefferedPSOutput {
	float4 color : SV_Target0;
};

//=========================================================================================
// Buffer
//=========================================================================================
/* deffered Textures */
Texture2D<float4> gAlbedo   : register(t0);
Texture2D<float4> gDepth    : register(t1);
Texture2D<float4> gNormal   : register(t2);
Texture2D<float4> gPosition : register(t3);
SamplerState gSampler : register(s0);

ConstantBuffer<Camera> gCamera : register(b0);
ConstantBuffer<Light>  gLight  : register(b1);

////////////////////////////////////////////////////////////////////////////////////////////
// main
////////////////////////////////////////////////////////////////////////////////////////////
DefferedPSOutput main(DefferedVSOutput input) {
	
	DefferedPSOutput output;
	
	
	float4 albed = Deffered::GetTextureAlbed(input.texcoord, gAlbedo, gSampler);
	
	if (albed.a == 0.0f) { //!< albedColor.aが0.0fの場合
		discard;
	}
	
	float3 normal = Deffered::GetTextureNormal(input.texcoord, gNormal, gSampler);
	float3 worldPos = Deffered::GetTextureWorldPos(input.texcoord, gPosition, gSampler);
	
	if (all(normal == 0.0f)) {
		output.color = albed;
		return output;
	}
	
	/* lambert */
	// ライトのパラメーターの設定
	float3 directionToLight = -gLight.direction;
	float factor            = 1.0f;
	
	if (gLight.lightType == LIGHT::POINT) {
		
		// direction
		directionToLight = normalize(gLight.position.xyz - worldPos.xyz);
			
		// factor
		float distance = length(worldPos.xyz - gLight.position.xyz);
		factor = pow(saturate(-distance / gLight.range + 1.0f), gLight.decay);
		
	} else if (gLight.lightType == LIGHT::SPOT) {
		
		// direction
		directionToLight = normalize(gLight.position.xyz - worldPos.xyz);
			
		// factor
		// angle
		float angle = dot(-directionToLight, gLight.direction);
		float falloffFactor = saturate((angle - gLight.angle) / (1.0f - gLight.angle));
		
		// length
		float distance = length(worldPos.xyz - gLight.position.xyz);
		float attenuationFactor = pow(saturate(-distance / gLight.range + 1.0f), gLight.decay);
		
		factor = attenuationFactor * falloffFactor;
		
	}
	
	output.color.a   = 1.0f;
	output.color.rgb = albed.rgb * HalfLambertReflection(normal, directionToLight) * gLight.color.rgb * gLight.intensity * factor;

	/* blinnPhong */
	
	float3 directionToCamera = normalize(gCamera.position.xyz - worldPos.xyz);
	
	output.color.rgb += BlinnPhong(worldPos, normal, directionToLight, directionToCamera, 100.0f) * gLight.intensity * factor;
	output.color.rgb = saturate(output.color.rgb);
	
	return output;
}