#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/UI/ISystemDebugGui.h>

//* thread
#include "AsyncTask.h"
#include "AsyncThread.h"

//* c++
#include <vector>
#include <queue>
#include <memory>
#include <mutex>

////////////////////////////////////////////////////////////////////////////////////////////
// AsyncThreadCollection class
////////////////////////////////////////////////////////////////////////////////////////////
class AsyncThreadCollection
	: public ISystemDebugGui {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	AsyncThreadCollection()  = default;
	~AsyncThreadCollection() { Term(); }

	void Init(uint32_t threadCount);

	void Term();

	void PushTask(const std::weak_ptr<AsyncTask>& task);

	void SystemDebugGui() override;

private:

	//=========================================================================================
	// private variables
	//=========================================================================================
	
	//* thread parameter *//

	std::vector<std::unique_ptr<AsyncThread>> threads_;

	std::mutex mutex_;

	//* task container *//

	std::queue<std::weak_ptr<AsyncTask>> tasks_;

	//=========================================================================================
	// private methods
	//=========================================================================================

	std::shared_ptr<AsyncTask> GetTask();

};
