#include "EditorEngineGameLoop.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/Editor/EditorEngine.h>
#include <Engine/Editor/Editors/DevelopEditor.h>
#include <Engine/Editor/Editors/OutlinerEditor.h>

////////////////////////////////////////////////////////////////////////////////////////////
// EditorEngineGameLoop class
////////////////////////////////////////////////////////////////////////////////////////////

void EditorEngineGameLoop::Init(GameLoop::Context* context) {
	context->SetProcess(GameLoop::Process::Init, std::nullopt, [this]() { InitEditor(); });
	context->SetProcess(GameLoop::Process::Term, std::nullopt, [this]() { TermEditor(); });
	context->SetProcess(GameLoop::Process::Update, std::nullopt, [this]() { UpdateEditor(); });
}

void EditorEngineGameLoop::Term() {
}

void EditorEngineGameLoop::InitEditor() {
	sEditorEngine->Init();
	sEditorEngine->RegisterEditor<DevelopEditor>();
	sEditorEngine->RegisterEditor<OutlinerEditor>();
}

void EditorEngineGameLoop::TermEditor() {
	sEditorEngine->Term();
}

void EditorEngineGameLoop::UpdateEditor() {
	sEditorEngine->UpdateEditor();
}
