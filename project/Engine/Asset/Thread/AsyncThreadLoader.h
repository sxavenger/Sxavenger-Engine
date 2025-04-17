#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* asset
#include "../Assets/BaseAsset.h"

//* engine
#include <Engine/System/DirectX/DirectXContext.h>
#include <Engine/System/UI/ISystemDebugGui.h>
#include <Engine/System/Runtime/Performance/RunTimeTracker.h>

//* c++
#include <functional>
#include <thread>
#include <array>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <memory>

////////////////////////////////////////////////////////////////////////////////////////////
// AsyncThreadLoader structure
////////////////////////////////////////////////////////////////////////////////////////////
struct AsyncThreadLoader
	: public DirectXThreadContext {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	void Init(const std::function<void()>& function);

	//* thread option *//

	void Execute();

	void Join() { thread_.join(); }

	bool Joinable() const { return thread_.joinable(); }

	//* task option *//

	void SetTask(const std::shared_ptr<BaseAsset>& task) { task_ = task; }

	//* debug option *//

	void SetImGuiTableCommand();

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	std::thread thread_;
	std::shared_ptr<BaseAsset> task_;

	RunTimeTracker runtime_;

};

////////////////////////////////////////////////////////////////////////////////////////////
// AsyncThreadLoaderCollection class
////////////////////////////////////////////////////////////////////////////////////////////
class AsyncThreadLoaderCollection
	: public ISystemDebugGui {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	AsyncThreadLoaderCollection() = default;
	~AsyncThreadLoaderCollection() { Term(); }

	void Init();

	void Term();

	void SystemDebugGui();

	void PushTask(const std::shared_ptr<BaseAsset>& task);

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* threads *//

	static const uint8_t kThreadCount_ = 2;
	std::array<AsyncThreadLoader, kThreadCount_> threads_;

	bool isExit_ = false;

	std::mutex              mutex_;
	std::condition_variable condition_;

	//* task queue *//

	std::queue<std::shared_ptr<BaseAsset>> tasks_;


};
