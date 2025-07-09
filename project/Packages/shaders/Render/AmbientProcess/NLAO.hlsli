#pragma once

//=========================================================================================
// buffers
//=========================================================================================

struct Parameter {
	uint2 samples;
	uint2 size;
	float radius;
	float strength;
};
ConstantBuffer<Parameter> gParameter : register(b0);
//!< b11に変更するかも

