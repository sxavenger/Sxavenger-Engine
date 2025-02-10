#pragma once

////////////////////////////////////////////////////////////////////////////////////////////
// methods
////////////////////////////////////////////////////////////////////////////////////////////

float4 BlendNormal(float4 src, float4 dst) {
	return float4(src.rgb * src.a + dst.rgb * (1.0f - src.a), src.a);
}

float4 BlendNormalAlpha(float4 src, float4 dst) {
	return float4(src.rgb * src.a + dst.rgb * (1.0f - src.a), saturate(src.a + dst.a));
}


float4 BlendAdd(float4 src, float4 dst) {
	return float4(src.rgb * src.a + dst.rgb, src.a);
}

union {

}
