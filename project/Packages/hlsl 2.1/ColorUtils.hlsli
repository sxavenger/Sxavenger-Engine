#pragma once

////////////////////////////////////////////////////////////////////////////////////////////
// convert methods
////////////////////////////////////////////////////////////////////////////////////////////

float4 ToColor4f(int4 color) {
	return float4(color / 255.0f);
}

float4 ToColor4f(uint code) {
	float4 result = (float4)0;

	result.r = ((code >> (8 * 3)) & 0xFF) / 255.0f;
	result.g = ((code >> (8 * 2)) & 0xFF) / 255.0f;
	result.b = ((code >> (8 * 1)) & 0xFF) / 255.0f;
	result.a = ((code >> (8 * 0)) & 0xFF) / 255.0f;

	return result;
}

////////////////////////////////////////////////////////////////////////////////////////////
// blend methods
////////////////////////////////////////////////////////////////////////////////////////////

float4 AlphaBlend(float4 base, float4 add) {
	float4 result;
	
	// alpha
	result.a = base.a + add.a * (1.0f - base.a);
	
	// color
	result.rgb = (base.rgb * base.a + add.rgb * add.a * (1.0f - base.a)) / result.a;
	
	return result;
	
}

float3 OverlayBlend(float3 baseColor, float3 blendColor) {
	float3 result;
	result.r = baseColor.r < 0.5 ? (2.0 * baseColor.r * blendColor.r) : (1.0 - 2.0 * (1.0 - baseColor.r) * (1.0 - blendColor.r));
	result.g = baseColor.g < 0.5 ? (2.0 * baseColor.g * blendColor.g) : (1.0 - 2.0 * (1.0 - baseColor.g) * (1.0 - blendColor.g));
	result.b = baseColor.b < 0.5 ? (2.0 * baseColor.b * blendColor.b) : (1.0 - 2.0 * (1.0 - baseColor.b) * (1.0 - blendColor.b));
	return result;
}