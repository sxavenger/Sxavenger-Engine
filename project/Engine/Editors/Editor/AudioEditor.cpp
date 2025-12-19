#include "AudioEditor.h"
SXAVENGER_ENGINE_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/Components/Component/Audio/AudioController.h>

//* externals
#include <imgui.h>

//////////////////////////////////////////////////////////////////////////////////////////
// AudioEditor class methods
//////////////////////////////////////////////////////////////////////////////////////////

void AudioEditor::Init() {
}

void AudioEditor::ShowMainMenu() {
	if (ImGui::BeginMenu("audio")) {
		ImGui::SeparatorText("audio");
		BaseEditor::MenuPadding();
		ShowAudioMenu();

		ImGui::EndMenu();
	}
}

void AudioEditor::ShowWindow() {
}

void AudioEditor::ShowAudioMenu() {
	for (const auto& [value, name] : magic_enum::enum_entries<AudioHandle::Tag>()) {
		float volume = sAudioController->GetVolume(value);
		if (ImGui::SliderFloat(name.data(), &volume, 0.0f, 1.0f, "%.2f")) {
			sAudioController->SetVolume(value, volume);
		}
	}
}
