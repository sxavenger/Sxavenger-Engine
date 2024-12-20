#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* task
#include "AsyncTask.h"

//* engine
#include <Engine/System/DirectX/DirectXContext.h>
#include <Engine/System/UI/ISystemDebugGui.h>
#include <Engine/System/Runtime/Performance/Performance.h>
#include <Engine/System/Runtime/Performance/DeltaTimePoint.h>

//* c++
#include <functional>
#include <thread>

////////////////////////////////////////////////////////////////////////////////////////////
// AsyncThread class
////////////////////////////////////////////////////////////////////////////////////////////
class AsyncThread
	: public DirectXThreadContext, public ISystemDebugGui {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	AsyncThread()  = default;
	~AsyncThread() { Term(); }

	virtual void Create(const std::function<void()>& threadFunction);

	void Term();

	void SystemDebugGui() override;

	//* execute task *//

	void ExecuteTask();

	//* getter *//

	bool IsTerm() const { return isTerm_; }

	//* setter *//

	void SetTask(const std::shared_ptr<AsyncTask>& task) { task_ = task; }

protected:

	//=========================================================================================
	// protected variables
	//=========================================================================================

	//* thread paramter *//

	std::thread thread_;

	bool isTerm_      = false;
	bool isAvailable_ = true;

	//* task *//

	std::shared_ptr<AsyncTask> task_;

	//* runtime *//

	RunTimeTracker runtime_;

};
