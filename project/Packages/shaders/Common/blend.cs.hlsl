
//=========================================================================================
// buffers
//=========================================================================================

Texture2D<float4>   gSrc : register(t0);
RWTexture2D<float4> gDst : register(u0);

struct Config {
	uint2 size;
};
ConstantBuffer<Config> gConfig : register(b0);

////////////////////////////////////////////////////////////////////////////////////////////
// main
////////////////////////////////////////////////////////////////////////////////////////////
[numthreads(16, 16, 1)]
void main(uint3 dispatchThreadId : SV_DispatchThreadID) {

	uint2 index = dispatchThreadId.xy;
	uint2 size  = gConfig.size;
	
	// �摜�T�C�Y�ȏ�̏ꍇ, �������݂����Ȃ�
	if (any(index >= size)) {
		return;
	}
	
	float4 src = gSrc.Load(uint3(index, 0));
	float4 dst = gDst[index];
	
	dst = dst * 0.7f + src * 0.3f;
	
	gDst[index] = dst;
	
}
