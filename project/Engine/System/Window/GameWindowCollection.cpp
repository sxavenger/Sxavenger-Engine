#include "GameWindowCollection.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/SxavengerSystem.h>

//* lib
#include <Lib/Environment.h>

//* external
#include <imgui.h>

////////////////////////////////////////////////////////////////////////////////////////////
// GameWindowCollection class methods
////////////////////////////////////////////////////////////////////////////////////////////

const std::weak_ptr<GameWindow> GameWindowCollection::CreateMainWindow(
	const Vector2ui& clientSize, const LPCWSTR& name, const Color4f& clearColor) {

	mainWindow_ = std::make_unique<GameWindow>();
	mainWindow_->Create(clientSize, name, SxavengerSystem::GetMainThreadContext(), nullptr, clearColor);
	return GetMainWindow();
}

const std::weak_ptr<GameWindow> GameWindowCollection::TryCreateSubWindow(
	const Vector2ui& clientSize, const LPCWSTR& name, const Color4f& clearColor) {

	if (!subWindows_.Contains(name)) {
		//!< containerにない場合, 生成
		std::shared_ptr<GameWindow> window = std::make_shared<GameWindow>();
		window->Create(clientSize, name, SxavengerSystem::GetMainThreadContext(), mainWindow_->GetHwnd(), clearColor);

		// windowを登録
		subWindows_.Emplace(name, std::move(window));
	}

	return GetSubWindow(name);
}

void GameWindowCollection::Term() {
	subWindows_.Clear();
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

	for (auto& window : subWindows_.GetMap()) {
		window.second->Present(false);
	}
}

const std::weak_ptr<GameWindow> GameWindowCollection::GetMainWindow() const {
	return mainWindow_;
}

const std::weak_ptr<GameWindow> GameWindowCollection::TryGetSubWindow(const LPCWSTR& name) const noexcept {
	if (!subWindows_.Contains(name)) {
		return {};
	}

	return subWindows_.At(name);
}

const std::weak_ptr<GameWindow> GameWindowCollection::GetSubWindow(const LPCWSTR& name) const {
	Assert(subWindows_.Contains(name), "sub winwdow is not found.");
	return subWindows_.At(name);
}

const GameWindow* GameWindowCollection::GetForcusWindow() const {
	HWND hwnd = GetForegroundWindow();

	if (hwnd == mainWindow_->GetHwnd()) {
		return mainWindow_.get();
	}

	for (const auto& window : subWindows_.GetMap()) {
		if (hwnd == window.second->GetHwnd()) {
			return window.second.get();
		}
	}

	return nullptr;
}

void GameWindowCollection::SystemDebugGui() {
	ImGui::SeparatorText("main window");
	if (ImGui::Selectable(ToString(kMainWindowTitle).c_str(), false)) {
		SetForegroundWindow(mainWindow_->GetHwnd());
	}

	ImGui::SeparatorText("sub window");
	for (const auto& sub : subWindows_.GetMap()) {
		if (ImGui::Selectable(ToString(sub.first).c_str(), false)) {
			SetForegroundWindow(sub.second->GetHwnd());
		}
	}
}

void GameWindowCollection::RemoveClosedSubWindow() {
	for (auto it = subWindows_.Begin(); it != subWindows_.End();) {
		if (!it->second->IsOpenWindow()) {
			it = subWindows_.Erase(it);
			continue;
		}

		it++;
	}
}
