#include "ProcessConsole.h"

////////////////////////////////////////////////////////////////////////////////////////////
// ProcessConsole class methods
////////////////////////////////////////////////////////////////////////////////////////////

void ProcessConsole::Init() {
	processPipeline_ = std::make_unique<ProcessPipeline>();
	processPipeline_->Init();
}

void ProcessConsole::Term() {
	processPipeline_.reset();
}

void ProcessConsole::UpdateConsole() {
	if (isDisplayProcessConsole_) {

	}
}

void ProcessConsole::ProcessXclipse(SxavengerFrame* frame) {
	frame;
}

void ProcessConsole::ProcessVisual(SxavengerFrame* frame) {
	frame;
}

void ProcessConsole::SetProcessPipeline(ProcessPipelineType type) {
	processPipeline_->SetPipeline(type);
}

void ProcessConsole::Dispatch(const Vector2ui& size) {
	processPipeline_->Dispatch(size);
}
