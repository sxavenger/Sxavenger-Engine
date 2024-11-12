//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "Rail.hlsli"

////////////////////////////////////////////////////////////////////////////////////////////
// main
////////////////////////////////////////////////////////////////////////////////////////////
[maxvertexcount(6)]
void main(line VSOutput input[2], inout TriangleStream<GSOutput> triStream) {

	// 四角形の頂点を計算
	GSOutput p0, p1, p2, p3 = (GSOutput)0;

	p0.position = mul(input[0].positionLeft, kViewProj);
	p1.position = mul(input[0].positionRight, kViewProj);
	p2.position = mul(input[1].positionLeft, kViewProj);
	p3.position = mul(input[1].positionRight, kViewProj);

	p0.texcoord = float2(0.0f, 1.0f);
	p1.texcoord = float2(1.0f, 1.0f);
	p2.texcoord = float2(0.0f, 0.0f);
	p3.texcoord = float2(1.0f, 0.0f);
	
	p0.color = input[0].color;
	p1.color = input[0].color;
	p2.color = input[1].color;
	p3.color = input[1].color;


	triStream.Append(p0);
	triStream.Append(p2);
	triStream.Append(p1);

	//triStream.RestartStrip();
	
	triStream.Append(p2);
	triStream.Append(p3);
	triStream.Append(p1);

	triStream.RestartStrip();
}