#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* c++
#include <cstdint>
#include <string>
#include <functional>

//-----------------------------------------------------------------------------------------
// forward
//-----------------------------------------------------------------------------------------
class AsyncThread;

////////////////////////////////////////////////////////////////////////////////////////////
// AsyncTask class
////////////////////////////////////////////////////////////////////////////////////////////
class AsyncTask {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// Status enum class
	////////////////////////////////////////////////////////////////////////////////////////////
	enum class Status : uint8_t {
		None,      // 初期状態、または未設定
		Pending,   // タスクがキューに入り、実行待ち
		Running,   // タスクが実行中
		Completed, // タスクが正常に完了
	};

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//* execute option *//

	virtual void Execute(const AsyncThread* thread);

	void SetFunction(const std::function<void(const AsyncThread*)>& function) { function_ = function; }

	//* task option *//

	const Status GetStatus() const { return status_; }

	void Wait(Status status = Status::Completed) const;

	bool IsCompleted() const { return status_ == Status::Completed; }

	void SetStatus(Status status) { status_ = status; }

	const std::string& GetTag() const { return tag_; }

	void SetTag(const std::string& tag) { tag_ = tag; }

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	Status status_ = Status::None;

	std::function<void(const AsyncThread*)> function_ = nullptr;

	std::string tag_ = "nuknown task";

	//!< Execute function または function_ のいずれかを使用する
};
