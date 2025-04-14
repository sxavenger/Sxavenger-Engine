#pragma once

//=========================================================================================
// buffers
//=========================================================================================

struct Parameter {
	uint2 size;     //!< blur size
	float threshold; //!< 閾値 note: float3にするかも
};
ConstantBuffer<Parameter> gParameter : register(b0);
//!< b11に変更するかも


