#include "ExampleGameLoop.h"
SXAVENGER_ENGINE_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/Configuration/Configuration.h>
#include <Engine/System/System.h>
#include <Engine/Components/Component/ComponentHelper.h>
#include <Engine/Render/FMainRender.h>
#include <Engine/Editors/EditorEngine.h>
#include <Engine/Editors/Editor/DevelopEditor.h>

////////////////////////////////////////////////////////////////////////////////////////////
// ExampleGameLoop class methods
////////////////////////////////////////////////////////////////////////////////////////////

void ExampleGameLoop::Init(Execution::Context* context) {
	context->SetProcess(Execution::Process::Init, std::nullopt, [this]() { InitSystem(); });
	context->SetProcess(Execution::Process::Term, std::nullopt, [this]() { TermSystem(); });
	context->SetProcess(Execution::Process::Update, std::nullopt, [this]() {
		if (auto develop = sEditorEngine->GetEditor<DevelopEditor>()) {
			if (!develop->IsProcessRequired()) {
				return;
			}
		}

		UpdateSystem();
	});
	context->SetProcess(Execution::Process::Render, std::nullopt, [this]() { RenderSystem(); });

	context->SetCondition(
		[this]() { return !System::ProcessMessage(); }
	);
}

void ExampleGameLoop::Term() {
}

void ExampleGameLoop::InitSystem() {

	main_ = System::CreateMainWindow(
		Configuration::GetConfig().resolution,
		L"[Sxavenger Engine / Sxx Engine] Example Window"
	).lock();

}

void ExampleGameLoop::TermSystem() {
}

void ExampleGameLoop::UpdateSystem() {

	//-----------------------------------------------------------------------------------------
	// Update
	//-----------------------------------------------------------------------------------------



	//-----------------------------------------------------------------------------------------
	// SystemUpdate...?
	//-----------------------------------------------------------------------------------------

	ComponentHelper::UpdateTransform();
	// todo: engine側のgameloopに移動.

	//-----------------------------------------------------------------------------------------
	// LateUpdate
	//-----------------------------------------------------------------------------------------

	//-----------------------------------------------------------------------------------------
	// final Update...?
	//-----------------------------------------------------------------------------------------

	ComponentHelper::UpdateSkinning();
	// todo: engine側のgameloopに移動.

	ComponentHelper::UpdateAudio3d();
}

void ExampleGameLoop::RenderSystem() {

	FMainRender::GetInstance()->Render(System::GetDirectQueueContext(), main_.get());

	main_->BeginRenderWindow(System::GetDirectQueueContext());
	main_->ClearWindow(System::GetDirectQueueContext());

	FMainRender::GetInstance()->PresentMain(System::GetDirectQueueContext());
	System::RenderImGui();

	main_->EndRenderWindow(System::GetDirectQueueContext());

}
