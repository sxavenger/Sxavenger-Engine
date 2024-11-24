//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "Simple.hlsli"

#define _DEFINE_CONSTANT_BUFFER

#ifdef _DEFINE_CONSTANT_BUFFER
struct Test {
	float2 v;
};
ConstantBuffer<Test> gTest : register(b0);
ConstantBuffer<Test> gTestA : register(b1);
#endif

////////////////////////////////////////////////////////////////////////////////////////////
// main
////////////////////////////////////////////////////////////////////////////////////////////
VSOutput main(VSInput input) {

	VSOutput output = (VSOutput)0;

	output.position = input.position;
	
#ifdef _DEFINE_CONSTANT_BUFFER
	output.position.xy += gTest.v;
#endif

	return output;

}