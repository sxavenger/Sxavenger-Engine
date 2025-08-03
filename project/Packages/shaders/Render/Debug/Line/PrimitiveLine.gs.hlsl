//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "../Primitive.hlsli"

//=========================================================================================
// constant variables
//=========================================================================================

static const float2 kDirections[4] = {
	float2(1.0f, 0.0f),  //!< +x
	float2(-1.0f, 0.0f), //!< -x
	float2(0.0f, 1.0f),  //!< +y
	float2(0.0f, -1.0f)  //!< -y
};

////////////////////////////////////////////////////////////////////////////////////////////
// methods
////////////////////////////////////////////////////////////////////////////////////////////

PSInput Convert(GSInput input) {
	PSInput output;
	
	output.position = input.position;
	output.color    = input.color;
	
	return output;
}

////////////////////////////////////////////////////////////////////////////////////////////
// main
////////////////////////////////////////////////////////////////////////////////////////////
[maxvertexcount(2 * 5)]
void main(line GSInput input[2], inout LineStream<PSInput> output) {

	// ndc空間上での隣接pixelの距離
	float2 dd = 2.0f / size;
	// f(x) = pixelからNDCへの変換 = x / (size.x, size.y) * 2.0f - 1.0f
	// f(x + 1) - f(x) = dd

	for (uint i = 0; i < 2; ++i) {
		output.Append(Convert(input[i]));
	}

	output.RestartStrip();

	if (input[0].thickness <= 0.0f || input[1].thickness <= 0.0f) {
		return; // 太さが0以下の場合は何もしない
	}

	for (uint i = 0; i < 4; ++i) {
		float2 direction = kDirections[i];

		for (uint j = 0; j < 2; ++j) {
			PSInput v = Convert(input[j]);
			v.position.xy += direction * dd * v.position.w * input[j].thickness;
			output.Append(v);
		}

		output.RestartStrip();
	}
}
