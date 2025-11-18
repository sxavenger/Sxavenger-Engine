struct Parameter {
	uint4 const0;
	uint4 const1;
	uint4 const2;
	uint4 const3;
};
ConstantBuffer<Parameter> gParameter : register(b0);

#define A_GPU 1
#define A_HLSL 1
#include "../../../../../../externals/ffx-fsr/ffx_a.h"

Texture2D<float4> gInput : register(t0);
SamplerState gSampler    : register(s0);

RWTexture2D<float4> gOutput : register(u0);

#define FSR_EASU_F 1
AF4 FsrEasuRF(AF2 p) {
	return gInput.GatherRed(gSampler, p, int2(0, 0));
}

AF4 FsrEasuGF(AF2 p) {
	return gInput.GatherGreen(gSampler, p, int2(0, 0));
}

AF4 FsrEasuBF(AF2 p) {
	return gInput.GatherBlue(gSampler, p, int2(0, 0));
}

#include "../../../../../../externals/ffx-fsr/ffx_fsr1.h"

////////////////////////////////////////////////////////////////////////////////////////////
// methods
////////////////////////////////////////////////////////////////////////////////////////////

void CurrFilter(AU2 pos) {
	AF3 c;
	FsrEasuF(c, pos, gParameter.const0, gParameter.const1, gParameter.const2, gParameter.const3);
	gOutput[pos] = float4(c, 1);
}

////////////////////////////////////////////////////////////////////////////////////////////
// main
////////////////////////////////////////////////////////////////////////////////////////////
[numthreads(64, 1, 1)]
void main(uint3 groupThreadId : SV_GroupThreadID, uint3 groupId : SV_GroupID) {

	// Do remapping of local xy in workgroup for a more PS-like swizzle pattern.
	AU2 gxy = ARmp8x8(groupThreadId.x) + AU2(groupId.x << 4u, groupId.y << 4u);
	
	CurrFilter(gxy);
	gxy.x += 8u;
	CurrFilter(gxy);
	gxy.y += 8u;
	CurrFilter(gxy);
	gxy.x -= 8u;
	CurrFilter(gxy);
	
}
