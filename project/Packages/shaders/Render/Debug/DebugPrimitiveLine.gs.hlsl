//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "DebugPrimitive.hlsli"

////////////////////////////////////////////////////////////////////////////////////////////
// main
////////////////////////////////////////////////////////////////////////////////////////////
[maxvertexcount(2 * 5)]
void main(line VSOutput input[2], inout LineStream<VSOutput> output) {

	output.Append(input[0]);
	output.Append(input[1]);
	output.RestartStrip();

	// ndc空間上での隣接pixelの距離
	float2 dd = 2.0f / size;
	// f(x) = pixelからNDCへの変換 = x / (size.x, size.y) * 2.0f - 1.0f
	// f(x + 1) - f(x) = dd

	// 太さの調整
	dd *= 0.6f;

	// x軸方向
	// +x
	for (uint i = 0; i < 2; ++i) {
		VSOutput v = input[i];
		v.position.xy += float2(dd.x, 0.0f) * v.position.w;
		output.Append(v);
	}
	output.RestartStrip();

	// -x
	for (uint i = 0; i < 2; ++i) {
		VSOutput v = input[i];
		v.position.xy -= float2(dd.x, 0.0f) * v.position.w;
		output.Append(v);
	}
	output.RestartStrip();

	// y軸方向
	// +y
	for (uint i = 0; i < 2; ++i) {
		VSOutput v = input[i];
		v.position.xy += float2(0.0f, dd.y) * v.position.w;
		output.Append(v);
	}
	output.RestartStrip();

	// -y
	for (uint i = 0; i < 2; ++i) {
		VSOutput v = input[i];
		v.position.xy -= float2(0.0f, dd.y) * v.position.w;
		output.Append(v);
	}
	output.RestartStrip();
	
}
