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
//! @brief 非同期スレッドプール管理クラス.
class AsyncThreadCollection
	: public ISystemDebugGui {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	AsyncThreadCollection() = default;
	~AsyncThreadCollection() { Shutdown(); }

	void Init();

	void SetTerminate();

	void Shutdown();

	//* task option *//

	//! @brief 指定した実行カテゴリのスレッドプールにタスクを追加する
	//! @param[in] execution 実行カテゴリ
	//! @param[in] task      追加するタスク
	void PushTask(AsyncExecution execution, const std::shared_ptr<AsyncTask>& task);

	//* debug gui *//

	//! @brief デバッグGUIの表示
	void SystemDebugGui() override;

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	std::array<AsyncThreadPool, kAsyncExecutionCount> pools_;

};
