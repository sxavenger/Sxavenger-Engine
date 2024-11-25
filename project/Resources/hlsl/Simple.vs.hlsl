//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "Simple.hlsli"

//#define _DEFINE_CONSTANT_BUFFER

#ifdef _DEFINE_CONSTANT_BUFFER
ConstantBuffer<TransformationMatrix> gTransform : register(b0);
#endif

////////////////////////////////////////////////////////////////////////////////////////////
// main
////////////////////////////////////////////////////////////////////////////////////////////
VSOutput main(VSInput input) {

	VSOutput output = (VSOutput)0;

	output.position = input.position;
	
#ifdef _DEFINE_CONSTANT_BUFFER
	output.position = mul(input.position, gTransform.mat);
#endif

	return output;

}