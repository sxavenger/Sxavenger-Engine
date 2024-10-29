//=========================================================================================
// buffers
//=========================================================================================
struct Well { // todo: vsにある構造体と合体
	float4x4 skeletonSpaceMatrix;
	float4x4 skeletonSpaceInverseTransposeMatrix;
};
StructuredBuffer<Well> gMatrixPalette : register(t0);

struct Vertex {
	float4 position;
	float2 texcoord;
	float3 normal;
};
StructuredBuffer<Vertex> gInputVertices : register(t1);

struct VertexInfluence {
	float4 weight;
	int4 index;
};
StructuredBuffer<VertexInfluence> gInfluences : register(t2);

struct SkinningInformation {
	uint numVertices;
};
ConstantBuffer<SkinningInformation> gInfomation : register(b0);

//=========================================================================================
// Output
//=========================================================================================
RWStructuredBuffer<Vertex> gOutputVertex : register(u0);

//=========================================================================================
// configs
//=========================================================================================
#define NUMTHREADS_X 1024
#define NUMTHREADS_Y 1
#define NUMTHREADS_Z 1

////////////////////////////////////////////////////////////////////////////////////////////
// main
////////////////////////////////////////////////////////////////////////////////////////////
[numthreads(NUMTHREADS_X, NUMTHREADS_Y, NUMTHREADS_Z)]
void main(uint3 dispatchThreadId : SV_DispatchThreadID) {
	
	// thread数xが今回のどこの頂点情報か扱う
	uint index = dispatchThreadId.x;
	
	// 頂点数以上の計算の場合, 情報を書き込まない
	if (index >= gInfomation.numVertices) {
		return;
	}
	
	// 必要な情報の取り出し
	Vertex          input     = gInputVertices[index];
	VertexInfluence influence = gInfluences[index];

	// skninng後の頂点を計算
	Vertex skinned;
	
	// texcoordはそのまま
	skinned.texcoord = input.texcoord;
	
	// positionの変換
	skinned.position  = mul(input.position, gMatrixPalette[influence.index.x].skeletonSpaceMatrix) * influence.weight.x;
	skinned.position += mul(input.position, gMatrixPalette[influence.index.y].skeletonSpaceMatrix) * influence.weight.y;
	skinned.position += mul(input.position, gMatrixPalette[influence.index.z].skeletonSpaceMatrix) * influence.weight.z;
	skinned.position += mul(input.position, gMatrixPalette[influence.index.w].skeletonSpaceMatrix) * influence.weight.w;
	skinned.position.w = 1.0f;
	
	// normalの変換
	skinned.normal  = mul(input.normal, (float3x3)gMatrixPalette[influence.index.x].skeletonSpaceInverseTransposeMatrix) * influence.weight.x;
	skinned.normal += mul(input.normal, (float3x3)gMatrixPalette[influence.index.y].skeletonSpaceInverseTransposeMatrix) * influence.weight.y;
	skinned.normal += mul(input.normal, (float3x3)gMatrixPalette[influence.index.z].skeletonSpaceInverseTransposeMatrix) * influence.weight.z;
	skinned.normal += mul(input.normal, (float3x3)gMatrixPalette[influence.index.w].skeletonSpaceInverseTransposeMatrix) * influence.weight.w;
	skinned.normal = normalize(skinned.normal);
	
	// skinning後の情報の書き込み
	gOutputVertex[index] = skinned;
	
}
