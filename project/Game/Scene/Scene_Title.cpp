#include "Scene_Title.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* other scene
#include "Scene_Game.h"

//* engine
#include <Engine/Console/SystemConsole.h>
#include <Lib/Adapter/Random/Random.h>
#include <Lib/Easing.h>

////////////////////////////////////////////////////////////////////////////////////////////
// Scene_Title class methods
////////////////////////////////////////////////////////////////////////////////////////////

void Scene_Title::Init() {

	teapot_ = std::make_unique<Teapot>();
	teapot_->Init();
	teapot_->SetToConsole();

	plane_ = std::make_unique<Plane>();
	plane_->Init();
	plane_->SetToConsole();

	animationTest_ = std::make_unique<AnimationPlayer>();
	animationTest_->Init();
	animationTest_->SetToConsole();

	camera_ = sSystemConsole->GetGameCamera();
	camera_->GetTransformBuffer().SetParent(&buffer_);

	buffer_.transform.translate = { 0.0f, 0.0f, -10.0f };
	buffer_.UpdateMatrix();
}

void Scene_Title::Term() {
}

void Scene_Title::Update() {
	if (Sxavenger::IsTriggerKey(DIK_SPACE)) {
		SetNextScene<Scene_Game>();
	}

	animationTest_->Update();

	if (Sxavenger::IsTriggerKey(DIK_S)) {
		timer_ = time_;
	}

	if (timer_.time != 0.0f) {
		timer_.SubtractDeltaTime();
		timer_.time = std::max(timer_.time, 0.0f);

		if (i > static_cast<uint32_t>(timer_.time * subdivision)) {
			i = static_cast<uint32_t>(timer_.time * subdivision);

			if (i % 3 == 0) {
				point_ = kOrigin3;

			} else {
				float strength = 2.0f;
				float strT = std::lerp(0.0f, strength, timer_.time / time_.time);

				point_ = { Random::Generate(-strT, strT), Random::Generate(-strT, strT), 0.0f };
			}

			
		}

		if (i % 3 == 0) {
			camera_->GetTransformBuffer().transform.translate = Lerp(camera_->GetTransformBuffer().transform.translate, point_, 0.03f);
		} else {
			camera_->GetTransformBuffer().transform.translate = Lerp(camera_->GetTransformBuffer().transform.translate, point_, 0.02f);
		}
		

	} else {
		camera_->GetTransformBuffer().transform.translate = Lerp(camera_->GetTransformBuffer().transform.translate, kOrigin3, 0.02f);
		i = static_cast<uint32_t>(time_.time * subdivision);
	}

	camera_->GetTransformBuffer().UpdateMatrix();
}

void Scene_Title::Draw() {
}
