#include "EditorEngineGameLoop.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/Editor/EditorEngine.h>
#include <Engine/Editor/Editors/EngineDeveloperEditor.h>
#include <Engine/Editor/Editors/RenderSceneEditor.h>
#include <Engine/Editor/Editors/OutlinerEditor.h>

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
	sEditorEngine->RegisterEditor<RenderSceneEditor>();
	sEditorEngine->RegisterEditor<OutlinerEditor>();
}

void EditorEngineGameLoop::TermEditor() {
	sEditorEngine->Term();
}

void EditorEngineGameLoop::UpdateEditor() {
	sEditorEngine->UpdateEditor();
}
