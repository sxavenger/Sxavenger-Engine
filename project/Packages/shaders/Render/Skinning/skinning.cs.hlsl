//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* geometry
#include "../../Content/Mesh.hlsli"

//* common
#include "../../Component/TransformComponent.hlsli"

//=========================================================================================
// config
//=========================================================================================

#define NUMTHREADS_X 32

//=========================================================================================
// Input Buffer
//=========================================================================================

StructuredBuffer<MeshVertex> gInputVertex : register(t0);
StructuredBuffer<TransformComponent> gPalette : register(t1);

struct VertexInfluence {
	float4 weight;
	int4   index;
};
StructuredBuffer<VertexInfluence> gInfluence : register(t2);

struct SkinningInformation {
	uint vertexSize;
};
ConstantBuffer<SkinningInformation> gInfo : register(b0);

//=========================================================================================
// Output Buffer
//=========================================================================================

RWStructuredBuffer<MeshVertex> gOutputVertex : register(u0);

//=========================================================================================
// variables
//=========================================================================================

static const uint kInfluenceCount = 4;

////////////////////////////////////////////////////////////////////////////////////////////
// main
////////////////////////////////////////////////////////////////////////////////////////////
[numthreads(NUMTHREADS_X, 1, 1)]
void main(uint3 dispatchThreadId : SV_DispatchThreadID) {
	
	// thread数xが今回のどこの頂点情報か扱う
	uint index = dispatchThreadId.x;
	
	// 頂点数以上の計算の場合, 情報を書き込まない
	if (index >= gInfo.vertexSize) {
		return;
	}
	
	// 必要な情報の取り出し
	MeshVertex input          = gInputVertex[index];
	VertexInfluence influence = gInfluence[index];
	
	// skiningの影響を受けない場合, そのまま
	//if (all(influence.index == 0) && all(influence.weight == 0.0f)) {
	//	gOutputVertex[index] = input;
	//	return;
	//}

	// skninng後の頂点を計算
	MeshVertex skinned = (MeshVertex)0;
	
	// texcoordはそのまま
	skinned.texcoord = input.texcoord;

	// positionの変換
	for (uint i = 0; i < kInfluenceCount; ++i) {
		// skiningの影響を受けない
		if (influence.index[i] == 0 && influence.weight[i] == 0.0f) {
			continue;
		}
		
		skinned.position += gPalette[influence.index[i]].Transform(input.position) * influence.weight[i];
		skinned.normal   += gPalette[influence.index[i]].TransformNormal(input.normal) * influence.weight[i];
	}

	skinned.position.w = 1.0f;
	skinned.normal     = normalize(skinned.normal);
	
	// skinning後の情報の書き込み
	gOutputVertex[index] = skinned;
	
}
