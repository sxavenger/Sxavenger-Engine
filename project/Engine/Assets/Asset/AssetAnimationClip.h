#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* asset
#include "BaseAsset.h"

//* engine
#include <Engine/Foundation.h>
#include <Engine/Graphics/Animation/Animation.h>

//* lib
#include <Lib/Adapter/Uuid/Uuid.h>

//* external
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////////////
// AssetAnimationClip class
////////////////////////////////////////////////////////////////////////////////////////////
//! @brief assimpのaiAnimationからアニメーションデータを読み込み, 保持するAsset
class AssetAnimationClip
	: public BaseAsset {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//* constructor / destructor *//

	AssetAnimationClip(const Uuid& uuid) : BaseAsset(uuid) {}

	~AssetAnimationClip() override = default;

	//* setup option *//

	//! @brief aiAnimationを解析し, アニメーションデータを構築する
	//! @param[in] animation 読み込み元のassimpアニメーション
	void Setup(const aiAnimation* animation);

	//* getter *//

	//! @brief 読み込んだアニメーションデータを取得する
	//! @return アニメーションデータへの参照
	const Animation& GetAnimation() const { return animation_; }

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	Animation animation_;

	//=========================================================================================
	// private methods
	//=========================================================================================

	//* [assimp] setup helper methods *//

	static TimePointd<TimeUnit::second> GetTime(double time, double ticksPerSeconds);

	static TransformAnimation LoadAnimation(const aiNodeAnim* aiNodeAnimation, double tickPerSeconds);

};

SXAVENGER_ENGINE_NAMESPACE_END
