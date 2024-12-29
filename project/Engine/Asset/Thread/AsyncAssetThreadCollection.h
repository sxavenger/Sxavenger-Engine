#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* thread
#include "AsyncAssetThread.h"

//* engine
#include <Engine/System/UI/ISystemDebugGui.h>

//* c++
#include <vector>
#include <mutex>
#include <memory>
#include <queue>

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

	void Init(uint32_t threadSize);

	void Term();

	void SystemDebugGui() override;

	//* task option *//

	void PushTask(const std::shared_ptr<BaseAsset>& task);

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* thread *//

	std::vector<std::unique_ptr<AsyncAssetThread>> threads_;

	std::mutex mutex_;

	//* tasks *//

	std::queue<std::weak_ptr<BaseAsset>> tasks_;

};
