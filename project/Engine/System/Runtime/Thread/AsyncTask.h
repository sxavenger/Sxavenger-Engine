#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/Foundation.h>

//* c++
#include <cstdint>
#include <string>
#include <functional>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN

//-----------------------------------------------------------------------------------------
// forward
//-----------------------------------------------------------------------------------------
class AsyncThread;

////////////////////////////////////////////////////////////////////////////////////////////
// AsyncTask class
////////////////////////////////////////////////////////////////////////////////////////////
//! @brief 非同期タスククラス.
class AsyncTask {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// Status enum class
	////////////////////////////////////////////////////////////////////////////////////////////
	enum class Status : uint8_t {
		None,      //!< 初期状態、または未設定
		Pending,   //!< タスクがキューに入り、実行待ち
		Running,   //!< タスクが実行中
		Completed, //!< タスクが正常に完了
	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// using
	////////////////////////////////////////////////////////////////////////////////////////////
	using Function = std::function<void(const AsyncThread*)>;


public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//* execute option *//

	virtual void Execute(const AsyncThread* thread);

	//! @brief タスクの実行関数を設定する
	void SetFunction(const Function& function) { function_ = function; }

	//* task option *//

	const Status GetStatus() const { return status_; }

	//! @brief 指定したステータスになるまで待機する
	void Wait(Status status = Status::Completed) const;

	//! @brief タスクが完了しているか確認する
	bool IsCompleted() const { return status_ == Status::Completed; }

	void SetStatus(Status status) { status_ = status; }

	const std::string& GetTag() const { return tag_; }

	void SetTag(const std::string& tag) { tag_ = tag; }

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	Status status_ = Status::None;

	Function function_ = nullptr;

	std::string tag_ = "nuknown task";

	//!< Execute function の継承 または function_ のいずれかを使用する
};

SXAVENGER_ENGINE_NAMESPACE_END
