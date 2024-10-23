//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "Skybox.hlsli"

//=========================================================================================
// Output
//=========================================================================================
struct PSOutput {
	float4 color : SV_Target0;
};

//=========================================================================================
// Buffer
//=========================================================================================
TextureCube<float4> gTexture : register(t0);
SamplerState gSampler : register(s0);

struct Parameter {
	float strength;
};
ConstantBuffer<Parameter> gParameter : register(b0);

//=========================================================================================
// methods
//=========================================================================================
float4 CreateVignette(float4 position, float2 size, float strength = 1.0f) {
	
	float2 uv = position.xy / size;
	float2 uvCenter = float2(0.5f, 0.5f);
	
	float2 t = abs(uv - uvCenter); //!< min: 0.0f, max: 0.5f
	
	float4 color = float4(0.0f, 0.0f, 0.0f, 0.0f); // viggnette color
	color.a = lerp(0.0f, 1.0f, pow((t.x + t.y), strength));
	
	return color;
}

float4 AlphaBlend(float4 base, float4 add) {
	float4 result;
	
	// alpha
	result.a = base.a + add.a * (1.0f - base.a);
	
	// color
	result.rgb = (base.rgb * base.a + add.rgb * add.a * (1.0f - base.a)) / result.a;
	
	return result;
	
}

////////////////////////////////////////////////////////////////////////////////////////////
// main
////////////////////////////////////////////////////////////////////////////////////////////
PSOutput main(VSOutput input) {
	
	PSOutput output;
	
	output.color = gTexture.Sample(gSampler, input.texcoord);
	
	if (gParameter.strength > 0.0f) {
		float4 vignetteColor = CreateVignette(input.position, float2(1280.0f, 720.0f), gParameter.strength);
		output.color = AlphaBlend(vignetteColor, output.color);
	}
	
	return output;
	
}