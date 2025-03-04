#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* render
#include "PostProcess/FPostProcessTextures.h"
#include "PostProcess/FPostProcess.h"

////////////////////////////////////////////////////////////////////////////////////////////
// FPostProcessSetting class
////////////////////////////////////////////////////////////////////////////////////////////
class FPostProcessSetting {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	FPostProcessSetting()  = default;
	~FPostProcessSetting() = default;

	//* process *//

	FPostProcess::Iterator AddProcess(FPostProcess* process);

	void RemoveProcess(FPostProcess* process);

	void EraseProcess(const FPostProcess::Iterator& it);

	//* execute process option *//

	bool CheckProcess() const { return !processes_.empty(); }

	void ExecuteProcess(const FPostProcess::ProcessContext& context);

	//* getter *//

	const FPostProcess::Contanier& GetProcesses() const { return processes_; }

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	FPostProcess::Contanier processes_;

};
