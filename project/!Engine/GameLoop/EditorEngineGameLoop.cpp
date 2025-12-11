#include "EditorEngineGameLoop.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/Editor/EditorEngine.h>
#include <Engine/Editor/Editors/DevelopEditor.h>
#include <Engine/Editor/Editors/InspectorEditor.h>
#include <Engine/Editor/Editors/RenderSceneEditor.h>
#include <Engine/Editor/Editors/HierarchyEditor.h>
#include <Engine/Editor/Editors/AssetEditor.h>
#include <Engine/Editor/Editors/AudioEditor.h>

////////////////////////////////////////////////////////////////////////////////////////////
// EditorEngineGameLoop class
////////////////////////////////////////////////////////////////////////////////////////////

void EditorEngineGameLoop::Init(GameLoop::Context* context) {
	context->SetProcess(GameLoop::Process::Init, std::nullopt, [this]() { InitEditor(); });
	context->SetProcess(GameLoop::Process::Term, std::nullopt, [this]() { TermEditor(); });
	context->SetProcess(GameLoop::Process::Update, std::nullopt, [this]() { UpdateEditor(); });
	context->SetProcess(GameLoop::Process::Render, std::nullopt, [this]() { RenderEditor(); });
}

void EditorEngineGameLoop::Term() {
}

void EditorEngineGameLoop::InitEditor() {
	sEditorEngine->Init();
	sEditorEngine->RegisterEditor<DevelopEditor>();
	sEditorEngine->RegisterEditor<InspectorEditor>();
	sEditorEngine->RegisterEditor<RenderSceneEditor>();
	sEditorEngine->RegisterEditor<HierarchyEditor>();
	sEditorEngine->RegisterEditor<AssetEditor>();
	sEditorEngine->RegisterEditor<AudioEditor>();
}

void EditorEngineGameLoop::TermEditor() {
	sEditorEngine->Term();
}

void EditorEngineGameLoop::UpdateEditor() {
	sEditorEngine->UpdateEditor();
}

void EditorEngineGameLoop::RenderEditor() {
	sEditorEngine->ExecuteEditorFunction<RenderSceneEditor>([&](RenderSceneEditor* editor) {
		editor->Render();
		SxavengerSystem::RecordLap("render [editor]");
	});
}
