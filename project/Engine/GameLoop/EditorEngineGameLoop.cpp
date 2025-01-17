#include "EditorEngineGameLoop.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/Editor/EditorEngine.h>
#include <Engine/Editor/Editors/EngineDeveloperEditor.h>

////////////////////////////////////////////////////////////////////////////////////////////
// EditorEngineGameLoop class
////////////////////////////////////////////////////////////////////////////////////////////

void EditorEngineGameLoop::Init(GameLoop::Context* context) {
	context->SetState(GameLoop::State::Init, std::nullopt, [this]() { InitEditor(); });
	context->SetState(GameLoop::State::Term, std::nullopt, [this]() { TermEditor(); });
	context->SetState(GameLoop::State::Update, std::nullopt, [this]() { UpdateEditor(); });
}

void EditorEngineGameLoop::Term() {
}

void EditorEngineGameLoop::InitEditor() {
	sEditorEngine->Init();
	sEditorEngine->RegisterEditor<EngineDeveloperEditor>();
}

void EditorEngineGameLoop::TermEditor() {
	sEditorEngine->Term();
}

void EditorEngineGameLoop::UpdateEditor() {
	sEditorEngine->UpdateEditor();
}
