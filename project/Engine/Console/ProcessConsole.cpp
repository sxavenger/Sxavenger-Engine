#include "ProcessConsole.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/Sxavenger.h>
#include <Engine/Console/SystemConsole.h>

////////////////////////////////////////////////////////////////////////////////////////////
// ProcessConsole class methods
////////////////////////////////////////////////////////////////////////////////////////////

void ProcessConsole::Init() {
	processPipeline_ = std::make_unique<ProcessPipeline>();
	processPipeline_->Init();

	layers_.emplace_back("test1");
	layers_.emplace_back("test2");
}

void ProcessConsole::Term() {
	processPipeline_.reset();
}

void ProcessConsole::UpdateConsole() {
	if (isDisplayProcessConsole_) {
		DisplayCanvas();
	}
}

void ProcessConsole::ProcessXclipse(SxavengerFrame* frame) {
	frame;
}

void ProcessConsole::ProcessVisual(SxavengerFrame* frame) {
	frame->BeginVisual();

	//VisualGlayscale(frame);

	frame->EndVisual();
}

void ProcessConsole::SetProcessPipeline(ProcessPipelineType type) {
	processPipeline_->SetPipeline(type);
}

void ProcessConsole::Dispatch(const Vector2ui& size) {
	processPipeline_->Dispatch(size);
}

void ProcessConsole::DisplayCanvas() {
	sSystemConsole->DockingConsole();
	ImGui::Begin("Canvas ## Process Console", nullptr, sSystemConsole->GetWindowFlag());

	for (auto it = layers_.begin(); it != layers_.end(); ++it) {
		SelectableLayer(it);
	}

	ImGui::End();
}

void ProcessConsole::DisplayLayer() {
}

void ProcessConsole::SelectableLayer(const LayerContainer::const_iterator& it) {

	ImGuiTreeNodeFlags flags
		= ImGuiTreeNodeFlags_OpenOnDoubleClick
		| ImGuiTreeNodeFlags_OpenOnArrow;

	if (ImGui::TreeNodeEx((*it).c_str(), flags)) {
		ImGui::TreePop();
	}

}

void ProcessConsole::VisualGlayscale(SxavengerFrame* frame) {
	frame->GetVisual()->NextResultBufferIndex();

	auto commandList = Sxavenger::GetCommandList();

	processPipeline_->SetPipeline(kVisual_Glayscale);

	commandList->SetComputeRootConstantBufferView(0, frame->GetConfigVirtualAddress());
	commandList->SetComputeRootDescriptorTable(1, frame->GetVisual()->GetPrevBuffer(1)->GetGPUHandleSRV());
	commandList->SetComputeRootDescriptorTable(2, frame->GetVisual()->GetResultBuffer()->GetGPUHandleUAV());

	Dispatch(frame->GetSize());
	
}
