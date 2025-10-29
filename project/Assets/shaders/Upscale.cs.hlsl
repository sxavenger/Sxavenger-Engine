//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------

//=========================================================================================
// buffers
//=========================================================================================

RWTexture2D<float4> gOutput : register(u0);
Texture2D<float4> gInput    : register(t0);

////////////////////////////////////////////////////////////////////////////////////////////
// methods
////////////////////////////////////////////////////////////////////////////////////////////

uint2 GetInputDimension() {
	uint width, height;
	gInput.GetDimensions(width, height);
	return uint2(width, height);
}

uint2 GetOutputDimension() {
	uint width, height;
	gOutput.GetDimensions(width, height);
	return uint2(width, height);
}

float4 Bilinear(uint2 index) {

	float2 scale = (float2)GetInputDimension() / (float2)GetOutputDimension();

	// 出力ピクセルの中心座標を入力テクスチャ空間に変換
	float2 position = (float2(index) + 0.5f) * scale - 0.5f;

	uint2 b  = (uint2)floor(position);
	float2 f = frac(position);

	uint2 leftTop     = clamp(b,               uint2(0, 0), GetInputDimension() - 1);
	uint2 rightTop    = clamp(b + uint2(1, 0), uint2(0, 0), GetInputDimension() - 1);
	uint2 leftBottom  = clamp(b + uint2(0, 1), uint2(0, 0), GetInputDimension() - 1);
	uint2 rightBottom = clamp(b + uint2(1, 1), uint2(0, 0), GetInputDimension() - 1);

	float4 c0 = lerp(gInput[leftTop],    gInput[rightTop],    f.x);
	float4 c1 = lerp(gInput[leftBottom], gInput[rightBottom], f.x);
	float4 c  = lerp(c0, c1, f.y);
	
	return c;
}

float CubicWeight(float x) {
	x = abs(x);
	if (x <= 1.0f) {
		return (1.5f * x - 2.5f) * x * x + 1.0f;
	} else if (x < 2.0f) {
		return ((-0.5f * x + 2.5f) * x - 4.0f) * x + 2.0f;
	} else {
		return 0.0f;
	}
}

float4 Bicubic(uint2 index) {

	float2 scale = (float2)GetInputDimension() / (float2)GetOutputDimension();

	// 出力ピクセルの中心座標を入力テクスチャ空間に変換
	float2 position = (float2(index) + 0.5f) * scale - 0.5f;

	uint2 b  = (uint2)floor(position);
	float2 f = frac(position);

	// compute 4 weights for x and y

	float2 w[4];

	for (uint i = 0; i < 4; ++i) {
		float dx = (float)i - 1 - f.x;
		float dy = (float)i - 1 - f.y;

		w[i].x = CubicWeight(dx);
		w[i].y = CubicWeight(dy);

	}

	float4 color = float4(0.0f, 0.0f, 0.0f, 0.0f);
	float weight = 0.0f;

	for (uint i = 0; i < 4; ++i) {
		for (uint j = 0; j < 4; ++j) {
			uint2 pos = clamp(b + uint2(i - 1, j - 1), uint2(0, 0), GetInputDimension() - 1);

			float wgh = w[i].x * w[j].y;
			float4 c  = gInput[pos];

			color  += c * wgh;
			weight += wgh;
		}
	}

	color /= weight;

	return color;
	
}

////////////////////////////////////////////////////////////////////////////////////////////
// main
////////////////////////////////////////////////////////////////////////////////////////////
[numthreads(16, 16, 1)]
void main(uint3 dispatchThreadId : SV_DispatchThreadID) {

	uint2 index = dispatchThreadId.xy;
	
	if (any(index >= GetOutputDimension())) {
		return;
	}

	gOutput[index] = Bilinear(index);
	gOutput[index] = Bicubic(index);
	
}
