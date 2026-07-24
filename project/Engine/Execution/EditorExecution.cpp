#include "EditorExecution.h"
SXAVENGER_ENGINE_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/Editors/EditorEngine.h>
#include <Engine/Editors/Editor/DevelopEditor.h>
#include <Engine/Editors/Editor/InspectorEditor.h>
#include <Engine/Editors/Editor/RenderSceneEditor.h>
#include <Engine/Editors/Editor/HierarchyEditor.h>
#include <Engine/Editors/Editor/AssetEditor.h>
#include <Engine/Editors/Editor/AudioEditor.h>

////////////////////////////////////////////////////////////////////////////////////////////
// EditorExecution class methods
////////////////////////////////////////////////////////////////////////////////////////////

void EditorExecution::Init(Execution::Context* context) {
	context->SetProcess(Execution::Process::Init, std::nullopt, [this]() { InitEditor(); });
	context->SetProcess(Execution::Process::Term, std::nullopt, [this]() { TermEditor(); });
	context->SetProcess(Execution::Process::Update, std::nullopt, [this]() { UpdateEditor(); });
	context->SetProcess(Execution::Process::Render, std::nullopt, [this]() { RenderEditor(); });
}

void EditorExecution::Term() {
}

void EditorExecution::InitEditor() {
	sEditorEngine->Init();
	sEditorEngine->RegisterEditor<DevelopEditor>();
	sEditorEngine->RegisterEditor<InspectorEditor>();
	sEditorEngine->RegisterEditor<RenderSceneEditor>();
	sEditorEngine->RegisterEditor<HierarchyEditor>();
	sEditorEngine->RegisterEditor<AssetEditor>();
	sEditorEngine->RegisterEditor<AudioEditor>();
}

void EditorExecution::TermEditor() {
	sEditorEngine->Term();
}

void EditorExecution::UpdateEditor() {
	sEditorEngine->UpdateEditor();
}

void EditorExecution::RenderEditor() {
	sEditorEngine->ExecuteEditorFunction<RenderSceneEditor>([&](RenderSceneEditor* editor) {
		editor->Render();
		System::RecordCpu("render [editor]");
	});
}
