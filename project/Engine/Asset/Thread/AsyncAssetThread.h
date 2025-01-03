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
#include <functional>

////////////////////////////////////////////////////////////////////////////////////////////
// AsyncAssetThread class
////////////////////////////////////////////////////////////////////////////////////////////
class AsyncAssetThread
	: public DirectXThreadContext, public ISystemDebugGui {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	AsyncAssetThread()  = default;
	~AsyncAssetThread() { Term(); }

	void Init(const std::function<std::shared_ptr<BaseAsset>()>& getter);

	void Term();

	void SystemDebugGui() override;

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* thread *//

	std::thread thread_;

	bool isTerm_ = false;

	//* task *//

	std::shared_ptr<BaseAsset> task_ = nullptr;

	//* parameter *//

	bool isAvailable_ = false;
	RunTimeTracker runtime_;

	//=========================================================================================
	// private methods
	//=========================================================================================

	void Execute();

};
