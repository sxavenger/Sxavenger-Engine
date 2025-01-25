#include "FPostProcessSetting.h"

////////////////////////////////////////////////////////////////////////////////////////////
// FPostProcessSetting class methods
////////////////////////////////////////////////////////////////////////////////////////////

FPostProcess::Iterator FPostProcessSetting::AddProcess(FPostProcess* process) {
	return processes_.emplace(processes_.end(), process);
}

void FPostProcessSetting::RemoveProcess(FPostProcess* process) {
	processes_.remove(process);
}

void FPostProcessSetting::EraseProcess(const FPostProcess::Iterator& it) {
	processes_.erase(it);
}
