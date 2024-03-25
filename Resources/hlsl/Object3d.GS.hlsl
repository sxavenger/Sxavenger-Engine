#include "Object3d.hlsli"

////////////////////////////////////////////////////////////////////////////////////////////
// メイン
////////////////////////////////////////////////////////////////////////////////////////////
[maxvertexcount(3)] // maxvertexcount(..) < max: 1204(32bit) / GSOutputのfloat数)
void main(
	triangle VSOutput input[3],
	inout TriangleStream<GSOutput> output) {
	
	
	for (uint i = 0; i < 3; ++i) {
		GSOutput element;
		
		element.position = input[i].position;
		element.worldPos = input[i].worldPos;
		element.normal   = input[i].normal;
		element.texcoord = input[i].texcoord;
		
		output.Append(element);
	}

}