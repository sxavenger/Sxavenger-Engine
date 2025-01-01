#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////////////////
// Framework SceneTextures class
////////////////////////////////////////////////////////////////////////////////////////////
class FSceneTextures { //!< GBuffer管理クラス
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// GBufferLayout enum class
	////////////////////////////////////////////////////////////////////////////////////////////
	enum class GBufferLayout {
		Normal,
		Material,
		Albedo_AO,

		// HACK: Deferredが完成したら削除するkamo
		Position,
	};

	//* [detail GBuffer Layout]
	//* Normal:    float3 normal, float _NOT_USED_1                                   [R8G8B8A8_UNORM]
	//* Material:  float metallic, float specular, float roughness, float _NOT_USED_1 [R8G8B8A8_UNORM]
	//* Albedo_AO: float3 albedo, float AO, float _NOT_USED_1                         [R32G32B32A32_FLOAT] //!< HDRのため32bit浮動小数点
	//* Position:  float3 position, float _NOT_USED_1                                 [R32G32B32A32_FLOAT] //!< HDRのため32bit浮動小数点

	//* [define detail]
	//* _NOT_USED_1: 未使用のデータ(初期値1.0f)

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	FSceneTextures()  = default;
	~FSceneTextures() = default;


private:

	//=========================================================================================
	// private variables
	//=========================================================================================

};
