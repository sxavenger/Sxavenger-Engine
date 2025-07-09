#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* thread
#include "AsyncTask.h"
#include "AsyncThread.h"

//* c++
#include <array>

////////////////////////////////////////////////////////////////////////////////////////////
// AsyncThreadCollection class
////////////////////////////////////////////////////////////////////////////////////////////
class AsyncThreadCollection {
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

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	std::array<AsyncThreadPool, kAsyncExecutionCount> pools_;

};
