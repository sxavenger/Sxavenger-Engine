#include "SceneController.h"

////////////////////////////////////////////////////////////////////////////////////////////
// SceneController class methods
////////////////////////////////////////////////////////////////////////////////////////////

void SceneController::Init(std::unique_ptr<const SceneFactory>&& factory) {
	factory_ = std::move(factory);
}

void SceneController::BeginState(const std::initializer_list<std::string>& names) {
	for (const std::string& name : names) {
		Push(name); //!< 遷移先シーンを追加
	}

	StartScene();
}

void SceneController::TransitionScene() {

	// 現在のシーンを取得
	BaseScene* scene = GetCurrentScene();

	if (scene == nullptr) {
		return; //!< シーンが存在しない場合は何もしない
	}
	
	if (!scene->IsTransition()) {
		return; //!< シーン遷移が発生していない場合は何もしない
	}

	const BaseScene::Transition transition = scene->GetTransition();

	switch (transition.destruction) {
		case BaseScene::Transition::Destruction::Single:
			scenes_.pop_back(); //!< 現在のシーンのみ破棄
			break;

		case BaseScene::Transition::Destruction::All:
			scenes_.clear(); //!< 全てのシーンを破棄
			break;
	}

	for (const std::string& name : transition.names) {
		Push(name); //!< 遷移先シーンを追加
	}

	StartScene();

}

void SceneController::UpdateScene() {
	// 現在のシーンを取得
	BaseScene* scene = GetCurrentScene();

	if (scene == nullptr) {
		return; //!< シーンが存在しない場合は何もしない
	}

	scene->Update();
}

void SceneController::LateUpdateScene() {
	// 現在のシーンを取得
	BaseScene* scene = GetCurrentScene();

	if (scene == nullptr) {
		return; //!< シーンが存在しない場合は何もしない
	}

	scene->LateUpdate();
}

bool SceneController::IsEmpty() const {
	return scenes_.empty();
}

BaseScene* SceneController::GetCurrentScene() const {
	if (scenes_.empty()) {
		return nullptr;
	}

	// 最後尾のシーンを取得
	return scenes_.back().get();
}

void SceneController::Push(const std::string& name) {
	std::unique_ptr<BaseScene> scene = factory_->CreateScene(name);
	scene->Init();

	scenes_.emplace_back(std::move(scene));
}

void SceneController::StartScene() {

	// 現在のシーンを取得
	BaseScene* scene = GetCurrentScene();

	if (scene == nullptr) {
		return; //!< シーンが存在しない場合は何もしない
	}

	scene->Start();
}
