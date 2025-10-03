#pragma once
/*
# reference
 - https://dev.epicgames.com/documentation/en-us/unreal-engine/bloom-in-unreal-engine
 - https://epicgames.ent.box.com/s/s86j70iamxvsuu6j35pilypficznec04
*/

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "../PostProcess.hlsli"

//=========================================================================================
// buffers
//=========================================================================================

struct Parameter {
	float intensity;
	float threshold;
	float radius;
};
ConstantBuffer<Parameter> gParameter : register(b0);
