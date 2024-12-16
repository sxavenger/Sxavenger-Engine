//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "Sprite.hlsli"

//=========================================================================================
// methods
//=========================================================================================

struct Projection {
	float4x4 proj;
};
ConstantBuffer<Projection> gProjection : register(b0);

////////////////////////////////////////////////////////////////////////////////////////////
// main
////////////////////////////////////////////////////////////////////////////////////////////
VSOutput main(VSInput input) {

	VSOutput output = (VSOutput)0;

	output.position = mul(mul(input.position, gInfo.transform2d), gProjection.proj);
	output.color    = input.color;
	output.texcoord = input.texcoord;

	return output;
}