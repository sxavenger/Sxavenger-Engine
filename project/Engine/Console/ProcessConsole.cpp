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

	layer1_ = std::make_unique<BaseProcessLayer>();
	layer1_->SetToConsole("aaa");

	layer2_ = std::make_unique<BaseProcessLayer>();
	layer2_->SetToConsole("bbb");

	layer3_ = std::make_unique<BaseProcessLayer>();
	layer3_->SetToConsole("ccc");
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
	frame->BeginXclipse();

	XclipseAtmoSphericScattering(frame);

	frame->EndXclipse();
}

void ProcessConsole::ProcessVisual(SxavengerFrame* frame) {
	frame->BeginVisual();

	//VisualGlayscale(frame);

	frame->EndVisual();
}

void ProcessConsole::SetLayer(BaseProcessLayer* layer) {
	layers_.emplace_back(layer);
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

	bool isSelected = false;

	if (selectedLayer_.has_value()) {
		isSelected = (it == selectedLayer_.value());
	}

	if (ImGui::Selectable((*it)->GetName().c_str(), isSelected)) {
		selectedLayer_ = it;
	}

	// drag and dropの処理
	if (ImGui::IsItemActive() && !ImGui::IsItemHovered()) {

		ImVec2 delta = ImGui::GetMouseDragDelta(0);

		if (delta.y > 0.0f) {
			 // 下方向へのドラッグ
			auto nextIt = std::next(it);

			if (nextIt != layers_.end()) {
				std::iter_swap(*it, *nextIt);  // it と nextIt を交換
			}

		} else if (delta.y < 0.0f) {
			 // 上方向へのドラッグ
			if (it != layers_.begin()) {
				auto prevIt = std::prev(it);
				std::iter_swap(*it, *prevIt);  // it と prevIt を交換
			}
		}

		ImGui::ResetMouseDragDelta();
	}
}

void ProcessConsole::XclipseAtmoSphericScattering(SxavengerFrame* frame) {

	auto commandList = Sxavenger::GetCommandList();

	processPipeline_->SetPipeline(kXclipse_AtmoSphericScattering);

	commandList->SetComputeRootConstantBufferView(0, frame->GetConfigVirtualAddress());
	commandList->SetComputeRootConstantBufferView(1, frame->GetCamera()->GetGPUVirtualAddress());
	commandList->SetComputeRootDescriptorTable(2, frame->GetXclipse()->GetTexture()->GetGPUHandleUAV());


	Dispatch(frame->GetSize());

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
