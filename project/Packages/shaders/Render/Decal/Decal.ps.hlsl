//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "DecalPass.hlsli"

//* library
#include "../../Library/ACES.hlsli"

//=========================================================================================
// buffers
//=========================================================================================

Texture2D<float4> gTexture : register(t0);
SamplerState gSampler : register(s0);

//!< GBuffer
Texture2D<float> gDepth : register(t1);

RWTexture2D<float4> gAlbedo      : register(u0);
RWTexture2D<float4> gNormal      : register(u1);
RWTexture2D<float4> gMaterialARM : register(u2);
RWTexture2D<uint2> gAddress      : register(u3);

////////////////////////////////////////////////////////////////////////////////////////////
// methods
////////////////////////////////////////////////////////////////////////////////////////////

bool IntersectDecal(float3 position, inout float2 texcoord) {

	float3 local = mul(float4(position, 1.0f), gTransform.GetMatInverse()).xyz;

	bool intersect = all(abs(local) <= 0.5f);

	if (intersect) {
		texcoord = local.xy + 0.5f;
		texcoord.y = 1.0f - texcoord.y; //!< テクスチャ座標系は左上が原点なので、y軸を反転する.
	}

	return intersect;

}

////////////////////////////////////////////////////////////////////////////////////////////
// main
////////////////////////////////////////////////////////////////////////////////////////////
[earlydepthstencil]
void main(DecalPSInput input) {

	//!< World座標の復元
	float depth     = gDepth.Load(int3(input.position.xy, 0));
	float2 viewport = input.position.xy / dimension * 2.0f - 1.0f;

	float3 position = gCamera.GetPosition(float2(viewport.x, -viewport.y), depth);

	float2 texcoord;
	bool intersect = IntersectDecal(position, texcoord);

	if (intersect) {
		//!< Decalの影響を受けるピクセル.
		gAddress[int2(input.position.xy)] = AddressBuffer.GetSprit();
		
		gAlbedo[int2(input.position.xy)].rgb = ACES::IDT_sRGB_AP1(gTexture.Sample(gSampler, texcoord).rgb);
		// TODO: 法線やスペキュラなども同様に更新する.
	}
	
}
