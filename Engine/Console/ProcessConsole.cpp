#include "ProcessConsole.h"

////////////////////////////////////////////////////////////////////////////////////////////
// ProcessConsole class methods
////////////////////////////////////////////////////////////////////////////////////////////

void ProcessConsole::Init() {
	pipeline_.Init();
}

void ProcessConsole::Term() {
}

void ProcessConsole::UpdateConsole() {
}

void ProcessConsole::ProcessXclipse(SxavengerFrame* frame) {
	frame;
}

void ProcessConsole::ProcessVisual(SxavengerFrame* frame) {
	frame;
}

void ProcessConsole::SetPipeline(ProcessPipelineType type) {
	pipeline_.SetPipeline(type);
}

void ProcessConsole::Dispatch(const Vector2ui& size) {
	pipeline_.Dispatch(size);
}
