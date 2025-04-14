//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "PostProcess.hlsli"

////////////////////////////////////////////////////////////////////////////////////////////
// common variables
////////////////////////////////////////////////////////////////////////////////////////////

static const uint kPointCount = 4;

////////////////////////////////////////////////////////////////////////////////////////////
// common methods
////////////////////////////////////////////////////////////////////////////////////////////

static float CatmullRomInterpolation(float p0, float p1, float p2, float p3, float t) {
	float t2 = t * t;
	float t3 = t2 * t;

	float e3 = -p0 + (p1 - p2) * 3.0f + p3; // t^3
	float e2 = p0 * 2.0f - p1 * 5.0f + p2 * 4.0f - p3; // t^2
	float e1 = -p0 + p2; // t
	float e0 = p1 * 2.0f;

	return (e3 * t3 + e2 * t2 + e1 * t + e0) * 0.5f;
}

////////////////////////////////////////////////////////////////////////////////////////////
// common structures
////////////////////////////////////////////////////////////////////////////////////////////

struct Point {
	float input;
	float output;
};

struct Table {

	//* member *//
	
	Point points[4];
	//!< pointはinputの昇順に並んでいることを前提としている
	
	//* methods *//
	
	float GetOutput(float input) {
		
		if (input <= points[0].input) { //!< 始点より小さい場合
			return points[0].output;
		}
		
		
		if (input >= points[3].input) { //!< 終点より大きい場合
			return points[3].output;
		}
		
		uint segmentIndex = 0;
		for (uint i = 0; i < kPointCount - 1; ++i) {
			if (input >= points[i].input && input < points[i + 1].input) {
				segmentIndex = i;
				break;
			}
		}
		
		// indexの決定
		uint index0 = segmentIndex - 1;
		uint index1 = segmentIndex;
		uint index2 = segmentIndex + 1;
		uint index3 = segmentIndex + 2;

		// fix index
		if (index1 == 0) {
			index0 = 0;
		}

		if (index3 >= kPointCount) {
			index3 = index2;
		}
		
		// inputTをセグメント内の位置に正規化
		float t = (input - points[index1].input) / (points[index2].input - points[index1].input);

		return CatmullRomInterpolation(points[index0].output, points[index1].output, points[index2].output, points[index3].output, t);
	}
	
};

//=========================================================================================
// buffers
//=========================================================================================

RWTexture2D<float4> gInput  : register(u0); //!< input texture
RWTexture2D<float4> gOutput : register(u1); //!< output texture

struct Parameter {
	Table r;
	Table g;
	Table b;
};
ConstantBuffer<Parameter> gParameter : register(b0);

////////////////////////////////////////////////////////////////////////////////////////////
// main
////////////////////////////////////////////////////////////////////////////////////////////
[numthreads(_NUM_THREADS_X, _NUM_THREADS_Y, 1)]
void main(uint3 dispatchThreadId : SV_DispatchThreadID) {

	uint2 index = dispatchThreadId.xy;
	
	if (CheckOverTexture(index)) {
		return; //!< texture size over
	}

	float4 color = gInput[index];
	
	color.r = gParameter.r.GetOutput(color.r);
	color.g = gParameter.g.GetOutput(color.g);
	color.b = gParameter.b.GetOutput(color.b);
	
	gOutput[index] = color;
	
}
