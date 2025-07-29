//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "../Primitive.hlsli"

//=========================================================================================
// constant variables
//=========================================================================================

static const float2 kDirections[4] = {
	float2(1.0f, 0.0f),  //!< +x
	float2(0.0f, -1.0f), //!< -y
	float2(-1.0f, 0.0f), //!< -x
	float2(0.0f, 1.0f),  //!< +y
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
[maxvertexcount(3 * 4)]
void main(point GSInput input[1], inout TriangleStream<PSInput> output) {

	// ndc空間上での隣接pixelの距離
	float2 dd = 2.0f / size;
	// f(x) = pixelからNDCへの変換 = x / (size.x, size.y) * 2.0f - 1.0f
	// f(x + 1) - f(x) = dd

	for (uint i = 0; i < 4; ++i) {

		output.Append(Convert(input[0]));

		for (uint j = 0; j < 2; ++j) {
			PSInput v = Convert(input[0]);
			v.position.xy += kDirections[(i + j) % 4] * dd * v.position.w * input[0].thickness;
			output.Append(v);
		}

		output.RestartStrip();
	}
	
}
