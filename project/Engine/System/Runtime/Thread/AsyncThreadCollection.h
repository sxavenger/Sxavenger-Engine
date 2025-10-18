#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* thread
#include "AsyncTask.h"
#include "AsyncThread.h"

//* engine
#include <Engine/System/UI/ISystemDebugGui.h>

//* c++
#include <array>

////////////////////////////////////////////////////////////////////////////////////////////
// AsyncThreadCollection class
////////////////////////////////////////////////////////////////////////////////////////////
class AsyncThreadCollection
	: public ISystemDebugGui {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	AsyncThreadCollection() = default;
	~AsyncThreadCollection() { Term(); }

	void Init();

	void Term();

	//* task option *//

	void PushTask(AsyncExecution execution, const std::shared_ptr<AsyncTask>& task);

	//* debug gui *//

	void SystemDebugGui() override;

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	std::array<AsyncThreadPool, kAsyncExecutionCount> pools_;

};
