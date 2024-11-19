#include "GameWindowCollection.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/SxavengerSystem.h>

//* external
#include <imgui.h>

////////////////////////////////////////////////////////////////////////////////////////////
// GameWindowCollection class methods
////////////////////////////////////////////////////////////////////////////////////////////

GameWindow* GameWindowCollection::CreateMainWindow(
	const Vector2ui& clientSize, const LPCWSTR& name, const Color4f& clearColor) {

	mainWindow_ = std::make_unique<GameWindow>();
	mainWindow_->Create(clientSize, name, SxavengerSystem::GetMainThreadContext(), nullptr, clearColor);
	return GetMainWindow();
}

const std::weak_ptr<GameWindow> GameWindowCollection::TryCreateSubWindow(
	const Vector2ui& clientSize, const LPCWSTR& name, const Color4f& clearColor) {

	if (!subWindows_.contains(name)) {
		//!< containerにない場合, 生成
		std::shared_ptr<GameWindow> window = std::make_shared<GameWindow>();
		window->Create(clientSize, name, SxavengerSystem::GetMainThreadContext(), mainWindow_->GetHwnd(), clearColor);

		// windowを登録
		subWindows_.emplace(name, std::move(window));
	}

	return GetSubWindow(name);
}

void GameWindowCollection::Term() {
	subWindows_.clear();
	mainWindow_.reset();
}

bool GameWindowCollection::ProcessMessage() {
	MSG msg = {};

	if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	if (msg.message == WM_QUIT) {
		return false;
	}

	RemoveClosedSubWindow();

	return true;
}

void GameWindowCollection::PresentAllWindow() {
	SxavengerSystem::TransitionAllocator();

	if (mainWindow_ != nullptr) {
		mainWindow_->Present(false);
	}

	for (auto& window : subWindows_) {
		window.second->Present(false);
	}
}

const std::weak_ptr<GameWindow> GameWindowCollection::TryGetSubWindow(const LPCWSTR& name) const noexcept {
	if (!subWindows_.contains(name)) {
		return {};
	}

	return subWindows_.at(name);
}

const std::weak_ptr<GameWindow> GameWindowCollection::GetSubWindow(const LPCWSTR& name) const {
	Assert(subWindows_.contains(name), "sub winwdow is not found.");
	return subWindows_.at(name);
}

const GameWindow* GameWindowCollection::GetForcusWindow() const {
	HWND hwnd = GetForegroundWindow();

	if (hwnd == mainWindow_->GetHwnd()) {
		return mainWindow_.get();
	}

	for (const auto& window : subWindows_) {
		if (hwnd == window.second->GetHwnd()) {
			return window.second.get();
		}
	}

	return nullptr;
}

void GameWindowCollection::RemoveClosedSubWindow() {
	for (auto it = subWindows_.begin(); it != subWindows_.end();) {
		if (!it->second->IsOpenWindow()) {
			it = subWindows_.erase(it);
			continue;
		}

		it++;
	}
}
