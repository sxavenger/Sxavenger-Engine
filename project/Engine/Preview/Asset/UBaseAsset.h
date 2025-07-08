#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/DirectX/DirectXContext.h>
#include <Engine/System/Runtime/Thread/AsyncTask.h>

//* lib
#include <Lib/CXXAttributeConfig.h>
#include <Lib/Adapter/Json/IJsonSerializer.h>
#include <Lib/Adapter/Uuid/Uuid.h>

////////////////////////////////////////////////////////////////////////////////////////////
// UBaseAsset class
////////////////////////////////////////////////////////////////////////////////////////////
class UBaseAsset
	: public AsyncTask {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//* constructor & destructor *//

	UBaseAsset()          = default;
	virtual ~UBaseAsset() = default;

	//* sasset option *//

	virtual json Serialize() const;

	virtual void Deserialize(const json& data);

	//* async task option *//

	bool IsComplete() const { return AsyncTask::GetStatus() == AsyncTask::Status::Completed; }

	void WaitComplete() const;

protected:

	//=========================================================================================
	// protected variables
	//=========================================================================================

	//* common *//

	Uuid uuid_;
	std::string name_;

	/* note: load方法
	 - contentでのfilepathからのロード
	 - .sassetファイルからのロード
		-> CommandContextが必要な場合(ex. texture)
	 > 拡張子で判断可能
	*/
};
