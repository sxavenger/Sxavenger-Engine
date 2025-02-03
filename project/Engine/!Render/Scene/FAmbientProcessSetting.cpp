#include "FAmbientProcessSetting.h"

////////////////////////////////////////////////////////////////////////////////////////////
// FAmbientProcessSetting class methods
////////////////////////////////////////////////////////////////////////////////////////////

FAmbientProcess::Iterator FAmbientProcessSetting::AddProcess(FAmbientProcess* process) {
	return processes_.emplace(processes_.end(), process);
}

void FAmbientProcessSetting::RemoveProcess(FAmbientProcess* process) {
	processes_.remove(process);
}

void FAmbientProcessSetting::EraseProcess(const FAmbientProcess::Iterator& it) {
	processes_.erase(it);
}

void FAmbientProcessSetting::ExecuteProcess(const FAmbientProcess::ProcessContext& context) {
	for (auto process : processes_) {
		process->Process(context);
	}
}
