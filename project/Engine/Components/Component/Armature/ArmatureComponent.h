#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* component
#include "../BaseComponent.h"

//* engine
#include <Engine/Foundation.h>
#include <Engine/Graphics/Animation/Skeleton.h>
#include <Engine/Assets/Asset/AssetSkeleton.h>
#include <Engine/Assets/Asset/AssetParameter.h>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////////////
// ArmatureComponent class
////////////////////////////////////////////////////////////////////////////////////////////
//! @brief スケルトンを保持し, アニメーションを適用してボーン姿勢を更新するcomponent
class ArmatureComponent final
	: public BaseComponent {
	//* unityの場合, armature一つ一つがEntityであるがhackとして実装
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	ArmatureComponent(EntityBehaviour* behaviour) : BaseComponent(behaviour) {}
	~ArmatureComponent() override = default;

	//* armature option *//

	//! @brief 使用するスケルトンAssetを設定し, ボーン構造を構築する
	//! @param[in] skeleton スケルトンAssetのuuid
	void SetSkeleton(const Uuid& skeleton);

	//! @brief 指定時刻のアニメーションをスケルトンへ適用する
	//! @param[in] animation 適用するアニメーション
	//! @param[in] time      再生時刻
	//! @param[in] isLoop    ループ再生するか
	void UpdateAnimation(const Animation& animation, TimePointd<TimeUnit::second> time, bool isLoop = true);

	//! @brief 2つのアニメーションを補間しながらブレンドしてスケルトンへ適用する
	//! @param[in] prevAnimation 遷移元アニメーション
	//! @param[in] prevTime      遷移元の再生時刻
	//! @param[in] prevIsLoop    遷移元をループ再生するか
	//! @param[in] currAnimation 遷移先アニメーション
	//! @param[in] currTime      遷移先の再生時刻
	//! @param[in] currIsLoop    遷移先をループ再生するか
	//! @param[in] t             補間係数 (0:遷移元 〜 1:遷移先)
	void TransitionAnimation(
		const Animation& prevAnimation, TimePointd<TimeUnit::second> prevTime, bool prevIsLoop,
		const Animation& currAnimation, TimePointd<TimeUnit::second> currTime, bool currIsLoop,
		float t
	);

	//! @brief 現在のスケルトン(ボーン姿勢)を取得する
	//! @return スケルトンへの参照
	const Skeleton& GetSkeleton() const { return skeleton_; }

	//* inspector *//

	//! @brief インスペクタにこのcomponentの編集UIを表示する (BaseComponentのoverride)
	void ShowComponentInspector() override;

	//* json option *//

	//! @brief このcomponentの状態をjsonへシリアライズする (BaseComponentのoverride)
	//! @return シリアライズしたjson
	json ParseToJson() const override;

	//! @brief jsonからこのcomponentの状態を復元する (BaseComponentのoverride)
	//! @param[in] data 入力元のjson
	void InputJson(const json& data) override;


	//=========================================================================================
	// public variables
	//=========================================================================================

	static inline const std::string kArmatureName = "Armature";

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	AssetParameter<AssetSkeleton> referenceSkeleton_;
	Skeleton skeleton_;


	//!< hack: animationをここでするのはどうか

	//=========================================================================================
	// private methods
	//=========================================================================================

	void PushBornLine(const Matrix4x4& mat, const std::vector<Joint>& joints);

};

SXAVENGER_ENGINE_NAMESPACE_END
