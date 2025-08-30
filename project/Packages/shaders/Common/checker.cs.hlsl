//=========================================================================================
// output buffer
//=========================================================================================

RWTexture2D<float4> gOutput : register(u0);

static const float4 kColor[4] = {
	float4(0.61f, 0.61f, 0.61f, 1.0f), // main: 0, sub: 0
	float4(0.77f, 0.77f, 0.77f, 1.0f), // main: 0, sub: 1
	float4(0.79f, 0.79f, 0.79f, 1.0f), // main: 1, sub: 0
	float4(1.0f, 1.0f, 1.0f, 1.0f)     // main: 1, sub: 1
};

////////////////////////////////////////////////////////////////////////////////////////////
// main
////////////////////////////////////////////////////////////////////////////////////////////
[numthreads(16, 16, 1)]
void main(uint3 dispatchThreadId : SV_DispatchThreadID) {

	uint2 size;
	gOutput.GetDimensions(size.x, size.y);

	if (any(dispatchThreadId.xy >= size)) {
		return;
	}

	uint2 main_position = dispatchThreadId.xy;
	uint2 main_size     = size.xy / 2; //!< main 2x2 grid

	uint main_checker = 0;
	main_checker += (main_position.x / main_size.x);
	main_checker += (main_position.y / main_size.y);
	uint main_color = main_checker % 2;

	uint2 sub_position = main_position.xy % main_size.xy;
	uint2 sub_size     = main_size.xy / 16; //!< sub 16x16 grid

	uint sub_checker = 0;
	sub_checker += (sub_position.x / sub_size.x);
	sub_checker += (sub_position.y / sub_size.y);
	uint sub_color = sub_checker % 2;

	uint color_index = (main_color << 1) | sub_color;

	gOutput[dispatchThreadId.xy] = kColor[color_index];
	
}
