#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/DirectX/DxObject/DxComputePipelineState.h>
#include <Engine/System/DirectX/Context/DirectXQueueContext.h>

//* engine
#include <Engine/Asset/Observer/AssetObserver.h>
#include <Engine/Asset/Assets/Blob/AssetBlob.h>

////////////////////////////////////////////////////////////////////////////////////////////
// Base CustomComputePipeline class
////////////////////////////////////////////////////////////////////////////////////////////
class BaseCustomComputePipeline {
public:

	//* asset option *//

	void SetAsset(const std::optional<AssetObserver<AssetBlob>>& blob);
	void CreateAsset(const std::filesystem::path& filepath);

	void ClearAsset();

	void ReloadAsset();

	virtual void RegisterBlob() = 0;

	virtual void ReloadAndSetPipeline(const DirectXQueueContext* context) = 0;

protected:

	//=========================================================================================
	// protected variables
	//=========================================================================================

	std::optional<AssetObserver<AssetBlob>> asset_;

	//=========================================================================================
	// protected methods
	//=========================================================================================

	bool CheckAsset() const;

	virtual void CheckAndReload() = 0;

};

////////////////////////////////////////////////////////////////////////////////////////////
// CustomComputePipeline class
////////////////////////////////////////////////////////////////////////////////////////////
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

