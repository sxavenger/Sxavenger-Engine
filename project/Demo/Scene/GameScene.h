#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/Module/Scene/BaseScene.h>
#include <Engine/Module/GameObject/GameObject.h>
#include <Engine/Module/Actor/PerspectiveCameraActor.h>
#include <Engine/Assets/Asset/AssetParameter.h>
#include <Engine/Assets/Asset/AssetAnimationClip.h>
#include <Engine/System/Runtime/Performance/DeltaTimePoint.h>

//* c++
#include <memory>

////////////////////////////////////////////////////////////////////////////////////////////
// Demo namespace
////////////////////////////////////////////////////////////////////////////////////////////
namespace Demo {

	////////////////////////////////////////////////////////////////////////////////////////////
	// GameScene class
	////////////////////////////////////////////////////////////////////////////////////////////
	//! @brief ゲーム本編のシーン(state). カメラ操作/キャラアニメーション/衝突を扱う
	class GameScene final
		: public SxxEngine::BaseScene {
	public:

		//=========================================================================================
		// public variables
		//=========================================================================================

		static constexpr const char* kSceneName = "Game"; //!< SceneFactoryへの登録名

		//=========================================================================================
		// public methods
		//=========================================================================================

		GameScene()           = default;
		~GameScene() override = default;

		//! @brief シーンの構築(カメラ/大気/キャラクター/衝突設定)を行う (BaseSceneのoverride)
		void Init() override;

		//! @brief シーン開始時の処理を行う (BaseSceneのoverride)
		void Start() override;

		//! @brief 毎フレームの更新(カメラ/アニメーション/入力)を行う (BaseSceneのoverride)
		void Update() override;

		//! @brief 描画前の遅延更新(大気の更新)を行う (BaseSceneのoverride)
		void LateUpdate() override;

	private:

		//=========================================================================================
		// private variables
		//=========================================================================================

		//* game object *//

		std::unique_ptr<PerspectiveCameraActor>    camera_;     //!< 操作可能なカメラ
		std::unique_ptr<SxxEngine::GameObject> atmosphere_; //!< 空(大気散乱)
		std::unique_ptr<SxxEngine::GameObject> human_;      //!< アニメーションするキャラクター

		//* animation *//

		SxxEngine::AssetParameter<SxxEngine::AssetAnimationClip> clip_; //!< 再生するアニメーションクリップ
		SxxEngine::DeltaTimePointd<TimeUnit::second>            time_; //!< アニメーション再生時刻

	};

} // namespace Demo
