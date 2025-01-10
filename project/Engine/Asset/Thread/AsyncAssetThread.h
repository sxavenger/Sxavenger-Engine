#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* asset
#include "../BaseAsset.h"

//* engine
#include <Engine/System/DirectX/DirectXContext.h>
#include <Engine/System/UI/ISystemDebugGui.h>
#include <Engine/System/Runtime/Performance/RunTimeTracker.h>

//* c++
#include <thread>
#include <array>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <memory>

////////////////////////////////////////////////////////////////////////////////////////////
// AsyncAssetThread structure
////////////////////////////////////////////////////////////////////////////////////////////
struct AsyncAssetThread
	: public DirectXThreadContext, public ISystemDebugGui {
public:

	//* member *//

	std::thread thread;
	std::shared_ptr<BaseAsset> task;

	RunTimeTracker runtime;

	//* method *//

	void Init();

	void SystemDebugGui() override;

	void Execute();

};

////////////////////////////////////////////////////////////////////////////////////////////
// AsyncAssetThreadCollection class
////////////////////////////////////////////////////////////////////////////////////////////
class AsyncAssetThreadCollection
	: public ISystemDebugGui {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	AsyncAssetThreadCollection()  = default;
	~AsyncAssetThreadCollection() { Term(); }

	void Init();

	void Term();

	void SystemDebugGui() override;

	void PushTask(const std::shared_ptr<BaseAsset>& task);

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* threads *//

	static const uint8_t kThreadCount_ = 4;
	std::array<AsyncAssetThread, kThreadCount_> threads_;

	bool isExit_ = false;

	std::mutex              mutex_;
	std::condition_variable condition_;

	//* task queue *//

	std::queue<std::weak_ptr<BaseAsset>> tasks_;

};
