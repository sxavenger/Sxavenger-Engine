//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "Simple.hlsli"

//=========================================================================================
// buffer
//=========================================================================================
ConstantBuffer<TransformationMatrix> gTransform : register(b0);
ConstantBuffer<Camera3d> gCamera : register(b1);
static const float4x4 kViewProj = mul(gCamera.viewMatrix, gCamera.projMatrix);

////////////////////////////////////////////////////////////////////////////////////////////
// main
////////////////////////////////////////////////////////////////////////////////////////////
PSInput main(Vertex input) {

	PSInput output = (PSInput)0;

	output.position = mul(mul(input.position, gTransform.mat), kViewProj);
	output.texcoord = input.texcoord;
	output.normal   = normalize(mul(input.normal, (float3x3)gTransform.matInverseTransopse));

	return output;

}