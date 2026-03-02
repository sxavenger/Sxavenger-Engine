#pragma once
/*
# reference
 - [UE5 - VolumetricFogドキュメント](https://dev.epicgames.com/documentation/ja-jp/unreal-engine/volumetric-fog-in-unreal-engine)
 - [レイマーチングVolumetricFog](https://zenn.dev/inpro/articles/73bc866b0d5d15)
*/

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "../PostProcess.hlsli"

//=========================================================================================
// buffers
//=========================================================================================

struct Parameter {
	float viewDistance;
	float startDistance;
};
ConstantBuffer<Parameter> gParameter : register(b0);
