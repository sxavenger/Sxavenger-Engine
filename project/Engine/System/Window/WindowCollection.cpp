#include "WindowCollection.h"
SXAVENGER_ENGINE_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/Utility/Convert.h>
#include <Engine/System/Utility/StreamLogger.h>
#include <Engine/System/UI/SxImGui.h>

//* external
#include <imgui.h>
#include <magic_enum.hpp>

//* c++
#include <ranges>

////////////////////////////////////////////////////////////////////////////////////////////
// WindowCollection class methods
////////////////////////////////////////////////////////////////////////////////////////////

std::weak_ptr<DirectXWindowContext> WindowCollection::CreateMainWindow(const Vector2ui& size, const std::wstring& name, const Color4f& color) {
	main_ = std::make_shared<DirectXWindowContext>();
	main_->Init(size, name, DirectXWindowContext::ProcessCategory::Application, color);

	hwnds_.emplace(main_->GetHwnd(), main_.get());

	return main_;
}

std::weak_ptr<DirectXWindowContext> WindowCollection::CreateSubWindow(const Vector2ui& size, const std::wstring& name, DirectXWindowContext::ProcessCategory category, const Color4f& color) {
	if (windows_.contains(name)) {
		StreamLogger::EngineLog(L"warning | window with name '" + name + L"' already exists.");
		return windows_.at(name);
	}

	auto window = std::make_shared<DirectXWindowContext>();
	window->Init(size, name, category, color);
	windows_.emplace(name, window);

	hwnds_.emplace(window->GetHwnd(), window.get());

	return window;
}

void WindowCollection::Term() {

	hwnds_.clear();

	if (main_) {
		main_.reset();
	}

	for (auto& window : windows_ | std::views::values) {
		window.reset();
	}
	windows_.clear();
}

bool WindowCollection::ProcessMessage() {
	MSG msg = {};

	while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	if (msg.message == WM_QUIT) {
		return false;
	}

	return true;
}

void WindowCollection::PresentWindows() {
	if (main_) {
		main_->Present();
	}

	for (const auto& window : windows_ | std::views::values) {
		window->Present();
	}
}

void WindowCollection::RemoveClosedWindow() {

	while (!removeQueue_.empty()) {
		removeQueue_.pop();
	}

	std::erase_if(windows_, [&](const auto& pair) {
		if (!pair.second->IsOpenWindow()) {
			hwnds_.erase(pair.second->GetHwnd());
			removeQueue_.push(pair.second);
			return true; // remove this window
		}

		return false;
	});
}


DirectXWindowContext* WindowCollection::GetFocusWindow() const {

	HWND focus = GetForegroundWindow();

	if (focus == nullptr || !hwnds_.contains(focus)) {
		return nullptr;
	}

	return hwnds_.at(focus);
}

void WindowCollection::SystemDebugGui() {
	ImGui::Dummy({ 240.0f, 0 });

	DirectXWindowContext* current = GetFocusWindow();

	ImGui::SeparatorText("main window");
	if (ImGui::Selectable(ToString(main_->GetName()).c_str(), main_.get() == current)) {
		SetForegroundWindow(main_->GetHwnd());
	}

	if (SxImGui::BeginHoveredTooltip()) {

		ImGui::Text("common info");
		ImGui::Separator();
		ImGui::Text("name:     %s",      ToString(main_->GetName()).c_str());
		ImGui::Text("size:     %u x %u", main_->GetSize().x, main_->GetSize().y);
		ImGui::Text("category: %s",      magic_enum::enum_name(main_->GetCategory()).data());
		ImGui::Dummy({ 0, 4 });
		
		ImGui::Text("window info");
		ImGui::Separator();
		ImGui::Text("hwnd:  0x%p", static_cast<void*>(main_->GetHwnd()));
		ImGui::Text("hinst: 0x%p", static_cast<void*>(main_->GetHinst()));
		ImGui::Dummy({ 0, 4 });

		ImGui::Text("DirectX info");
		ImGui::Separator();
		ImGui::Text("color space: %s", magic_enum::enum_name(main_->GetColorSpace()).data());

		SxImGui::EndHoveredTooltip();
	}

	ImGui::SeparatorText("sub window");
	for (const auto& window : windows_ | std::views::values) {
		if (ImGui::Selectable(ToString(window->GetName()).c_str(), window.get() == current)) {
			SetForegroundWindow(window->GetHwnd());
		}
	}
}
