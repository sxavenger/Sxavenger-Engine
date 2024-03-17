#include "Particle.hlsli"

struct Material {
	float4 color;
	float4x4 uvTransform;
	int lambertType;
	int enableBlinnPhong;
	float specPow;
};
ConstantBuffer<Material> gMaterial : register(b0);

Texture2D<float4> gTexture : register(t0);
SamplerState gSampler : register(s0);

struct PSOutput {
	float4 color : SV_TARGET0;
};

PSOutput main(VSOutput input) {
	
	PSOutput output;
    
    // textureColor
	float4 transformUV = mul(float4(input.texcoord, 0.0f, 1.0f), gMaterial.uvTransform);
	float4 textureColor = gTexture.Sample(gSampler, transformUV.xy);
    
	float4 defaultColor = gMaterial.color * textureColor * input.color;
	output.color = defaultColor;
	
	if (output.color.a == 0.0f) {
		discard;
	}

	return output;
}