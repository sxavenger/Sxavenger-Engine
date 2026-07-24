//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "LineGeometry.hlsli"

//=========================================================================================
// constant variables
//=========================================================================================

static const float2 kDirections[4] = { //!< Screenspace上での方向
	float2(1.0f, 0.0f),  //!< +x
	float2(-1.0f, 0.0f), //!< -x
	float2(0.0f, 1.0f),  //!< +y
	float2(0.0f, -1.0f)  //!< -y
};

////////////////////////////////////////////////////////////////////////////////////////////
// main
////////////////////////////////////////////////////////////////////////////////////////////
[maxvertexcount(6 * 5)]
void main(triangle GeometryPSInput input[3], inout LineStream<GeometryPSInput> output) {

	// ndc空間上での隣接pixelの距離
	float2 dd = 2.0f / dimension;
	// f(x) = pixelからNDCへの変換 = x / (dimension.x, dimension.y) * 2.0f - 1.0f
	// f(x + 1) - f(x) = dd

	for (uint i = 0; i < 3; ++i) {

		// 隣接する頂点のインデックス
		uint curr = i;
		uint next = (i + 1) % 3;

		// 頂点情報の取得
		GeometryPSInput v0 = input[curr];
		GeometryPSInput v1 = input[next];

		// 基底の線分の出力
		output.Append(v0);
		output.Append(v1);
		output.RestartStrip();

		if (thickness <= 0.0f) {
			continue; // 太さが0以下の場合は何もしない
		}
		
		for (uint i = 0; i < 4; ++i) {
			float2 direction = kDirections[i];
			
			// 太さ分の線分の出力
			GeometryPSInput v = v0;
			v.position.xy += direction * dd * v.position.w * thickness;
			output.Append(v);
			
			v = v1;
			v.position.xy += direction * dd * v.position.w * thickness;
			output.Append(v);
			
			output.RestartStrip();
		}

	}
	
}
