#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* render
#include "AmbientProcess/FAmbientProcess.h"

////////////////////////////////////////////////////////////////////////////////////////////
// FAmbientProcessSetting class
////////////////////////////////////////////////////////////////////////////////////////////
class FAmbientProcessSetting {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	FAmbientProcessSetting()  = default;
	~FAmbientProcessSetting() = default;

	//* process *//

	FAmbientProcess::Iterator AddProcess(FAmbientProcess* process);

	void RemoveProcess(FAmbientProcess* process);

	void EraseProcess(const FAmbientProcess::Iterator& it);

	//* execute process option *//

	bool CheckProcess() const { return !processes_.empty(); }

	void ExecuteProcess(const FAmbientProcess::ProcessContext& context);

	//* getter *//

	const FAmbientProcess::Container& GetProcesses() const { return processes_; }

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	FAmbientProcess::Container processes_;

};
