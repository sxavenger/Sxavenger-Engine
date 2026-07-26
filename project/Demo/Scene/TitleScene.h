#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/Module/Scene/BaseScene.h>
#include <Engine/Module/GameObject/GameObject.h>
#include <Engine/Module/Actor/PerspectiveCameraActor.h>

//* c++
#include <memory>

////////////////////////////////////////////////////////////////////////////////////////////
// Demo namespace
////////////////////////////////////////////////////////////////////////////////////////////
namespace Demo {

	////////////////////////////////////////////////////////////////////////////////////////////
	// TitleScene class
	////////////////////////////////////////////////////////////////////////////////////////////
	//! @brief タイトル画面のシーン(state). 入力待ちでGameSceneへ遷移する
	class TitleScene final
		: public SxxEngine::BaseScene {
	public:

		//=========================================================================================
		// public variables
		//=========================================================================================

		static constexpr const char* kSceneName = "Title"; //!< SceneFactoryへの登録名

		//=========================================================================================
		// public methods
		//=========================================================================================

		TitleScene()           = default;
		~TitleScene() override = default;

		//! @brief シーンの構築(カメラ/大気の生成)を行う (BaseSceneのoverride)
		void Init() override;

		//! @brief シーン開始時の処理を行う (BaseSceneのoverride)
		void Start() override;

		//! @brief 毎フレームの更新. 決定入力でGameSceneへの遷移を要求する (BaseSceneのoverride)
		void Update() override;

		//! @brief 描画前の遅延更新(大気の更新)を行う (BaseSceneのoverride)
		void LateUpdate() override;

	private:

		//=========================================================================================
		// private variables
		//=========================================================================================

		//* game object *//

		std::unique_ptr<PerspectiveCameraActor>    camera_;     //!< 背景描画用のカメラ
		std::unique_ptr<SxxEngine::GameObject> atmosphere_; //!< 空(大気散乱)

	};

} // namespace Demo
