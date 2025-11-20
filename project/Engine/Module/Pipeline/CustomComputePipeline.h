#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/DirectX/DxObject/DxComputePipelineState.h>
#include <Engine/System/DirectX/Context/DirectXQueueContext.h>

//* engine
#include <Engine/Preview/Content/ContentBlob.h>
#include <Engine/Preview/Content/ContentObserver.h>

////////////////////////////////////////////////////////////////////////////////////////////
// Base CustomComputePipeline class
////////////////////////////////////////////////////////////////////////////////////////////
//! @brief カスタムComputePipeline基底クラス
class BaseCustomComputePipeline {
public:

	//* asset option *//

	void SetContent(const std::shared_ptr<ContentBlob>& blob);
	void CreateContent(const std::filesystem::path& filepath);

	void ClearContent();

	void ReloadContent();

	virtual void RegisterBlob() = 0;

	virtual void ReloadAndSetPipeline(const DirectXQueueContext* context) = 0;

protected:

	//=========================================================================================
	// protected variables
	//=========================================================================================

	std::optional<ContentObserver<ContentBlob>> content_;

	//=========================================================================================
	// protected methods
	//=========================================================================================

	bool CheckAsset() const;

	virtual void CheckAndReload() = 0;

};

////////////////////////////////////////////////////////////////////////////////////////////
// CustomComputePipeline class
////////////////////////////////////////////////////////////////////////////////////////////
//! @brief Asset管理付きComputePipelineクラス
class CustomComputePipeline
	: public DxObject::ComputePipelineState, public BaseCustomComputePipeline {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	CustomComputePipeline()  = default;
	~CustomComputePipeline() = default;

	//* asset option *//

	void RegisterBlob() override;

	void ReloadAndSetPipeline(const DirectXQueueContext* context) override;

private:

	//=========================================================================================
	// private methods
	//=========================================================================================

	void CheckAndReload() override;

};

////////////////////////////////////////////////////////////////////////////////////////////
// CustomReflectionComputePipeline class
////////////////////////////////////////////////////////////////////////////////////////////
//! @brief Asset管理付きReflectionComputePipelineクラス
class CustomReflectionComputePipeline
	: public DxObject::ReflectionComputePipelineState, public BaseCustomComputePipeline {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	CustomReflectionComputePipeline()  = default;
	~CustomReflectionComputePipeline() = default;

	//* asset option *//

	void RegisterBlob() override;

	void ReloadAndSetPipeline(const DirectXQueueContext* context) override;

private:

	//=========================================================================================
	// private methods
	//=========================================================================================

	void CheckAndReload() override;

};

