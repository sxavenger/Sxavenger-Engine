#pragma once

//=========================================================================================
// buffers
//=========================================================================================

struct Parameter {
	uint2 size;     //!< blur size
	float threshold; //!< è‡’l note: float3‚É‚·‚é‚©‚à
};
ConstantBuffer<Parameter> gParameter : register(b0);
//!< b11‚É•ÏX‚·‚é‚©‚à
