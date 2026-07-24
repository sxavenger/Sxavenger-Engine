#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* async
#include "AsyncExecutionTask.h"
#include "AsyncExecutionThread.h"

//* engine
#include <Engine/Foundation.h>

//* c++
#include <memory>
#include <queue>
#include <array>
#include <list>
#include <mutex>
#include <condition_variable>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////////////
// Async namespace
////////////////////////////////////////////////////////////////////////////////////////////
namespace Async {

	////////////////////////////////////////////////////////////////////////////////////////////
	// ExecutionThreadPool class
	////////////////////////////////////////////////////////////////////////////////////////////
	//! @brief Execution種別(優先度)ごとにワーカースレッドとタスクキューを管理する非同期スレッドプール
	class ExecutionThreadPool {
	public:

		////////////////////////////////////////////////////////////////////////////////////////////
		// TaskQueue class
		////////////////////////////////////////////////////////////////////////////////////////////
		//! @brief Execution種別ごとにタスクを保持し, 優先度カスケードで取り出すキュー
		class TaskQueue {
		public:

			////////////////////////////////////////////////////////////////////////////////////////////
			// using
			////////////////////////////////////////////////////////////////////////////////////////////

			using Queue = std::queue<std::shared_ptr<ExecutionTask>>;
			 //!< 各Executionごとにタスクを管理するキューコンテナ.

		public:

			//=========================================================================================
			// public methods
			//=========================================================================================

			void Push(const std::shared_ptr<ExecutionTask>& task);

			std::shared_ptr<ExecutionTask> Pop(Execution execution);

			bool Empty() const;

			bool HasTask(Execution execution) const;

			Execution GetFrontExecution() const;

			size_t GetTaskCount(Execution execution) const;

			size_t GetTotalTaskCount() const;

		private:

			//=========================================================================================
			// private variables
			//=========================================================================================

			std::array<Queue, static_cast<uint8_t>(Execution::Cpu) + 1> queue_;

		};

		////////////////////////////////////////////////////////////////////////////////////////////
		// ThreadCondition class
		////////////////////////////////////////////////////////////////////////////////////////////
		//! @brief Execution種別ごとの条件変数で, ワーカースレッドの待機/起床を制御するクラス
		class ThreadCondition {
		public:

			//=========================================================================================
			// public methods
			//=========================================================================================

			void Notify(Execution execution);

			void NotifyAll();

			void Wait(Execution execution, const std::function<bool()>& predicate);

		private:

			//=========================================================================================
			// private variables
			//=========================================================================================

			std::mutex mutex_;
			std::array<std::condition_variable, static_cast<uint8_t>(Execution::Cpu) + 1> conditions_;

		};

	public:

		//=========================================================================================
		// public methods
		//=========================================================================================

		//! @brief 各Executionのワーカースレッドを生成し, プールを起動する
		void Init();

		//! @brief 全スレッドへ終了を通知する
		//! @param[in] isWaitForQueue trueならキューが空になるまで待ってから終了通知する
		void NotifyTerminate(bool isWaitForQueue = false);

		//! @brief 終了通知を行い, 全スレッドを破棄する
		void Shutdown();

		//* task option *//

		//! @brief タスクをキューへ投入し, 処理可能なスレッドを起こす
		//! @param[in] task 投入するタスク
		void PushTask(const std::shared_ptr<ExecutionTask>& task);

		//* gui option *//

		//! @brief キュー/スレッドの状態をImGuiで表示する (デバッグ用)
		void DebugGui();

	private:

		//=========================================================================================
		// private variables
		//=========================================================================================

		std::list<ExecutionThread> threads_;

		TaskQueue queue_;
		ThreadCondition condition_;

		bool isTerminate_ = false;

		//=========================================================================================
		// private methods
		//=========================================================================================

		std::shared_ptr<ExecutionTask> GetTask(const ExecutionThread* thread);

		void CreateThread(Execution execution, size_t count);

	};

}

SXAVENGER_ENGINE_NAMESPACE_END

