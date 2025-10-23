#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/DirectX/DxObject/DxGraphicsPipelineState.h>
#include <Engine/System/DirectX/Context/DirectXQueueContext.h>

//* engine
#include <Engine/Preview/Content/UContentBlob.h>
#include <Engine/Preview/Content/UContentObserver.h>

////////////////////////////////////////////////////////////////////////////////////////////
// Base CustomGraphicsPipeline class
////////////////////////////////////////////////////////////////////////////////////////////
//! @brief カスタムGraphicsPipeline基底クラス
class BaseCustomGraphicsPipeline {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	BaseCustomGraphicsPipeline()          = default;
	virtual ~BaseCustomGraphicsPipeline() = default;

	//* asset option *//

	void SetContent(const std::shared_ptr<UContentBlob>& blob, DxObject::GraphicsShaderType type);
	void CreateContent(const std::filesystem::path& filepath, DxObject::GraphicsShaderType type);

	void ClearContent();

	void ReloadContent();

	virtual void RegisterBlob() = 0;

	virtual void ReloadAndSetPipeline(const DirectXQueueContext* context, const D3D12_VIEWPORT& viewport, const D3D12_RECT& rect) = 0;
	virtual void ReloadAndSetPipeline(const DirectXQueueContext* context, const Vector2ui& windowSize = kMainWindowSize) = 0;

protected:

	//=========================================================================================
	// protected variables
	//=========================================================================================

	std::array<std::optional<UContentObserver<UContentBlob>>, static_cast<uint8_t>(DxObject::GraphicsShaderType::ps) + 1> contents_;

	//=========================================================================================
	// protected methods
	//=========================================================================================

	bool CheckAsset() const;

	virtual void CheckAndReload() = 0;

};

////////////////////////////////////////////////////////////////////////////////////////////
// CustomGraphicsPipeline class
////////////////////////////////////////////////////////////////////////////////////////////
//! @brief Asset管理付きGraphicsPipelineクラス
class CustomGraphicsPipeline
	: public DxObject::GraphicsPipelineState, public BaseCustomGraphicsPipeline {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	CustomGraphicsPipeline()  = default;
	~CustomGraphicsPipeline() = default;

	void RegisterBlob() override;

	void ReloadAndSetPipeline(const DirectXQueueContext* context, const D3D12_VIEWPORT& viewport, const D3D12_RECT& rect) override;
	void ReloadAndSetPipeline(const DirectXQueueContext* context, const Vector2ui& windowSize = kMainWindowSize) override;

private:

	//=========================================================================================
	// private methods
	//=========================================================================================

	void CheckAndReload() override;

};

////////////////////////////////////////////////////////////////////////////////////////////
// CustomReflectionGraphicsPipeline class
////////////////////////////////////////////////////////////////////////////////////////////
//! @brief Asset管理付きReflectionGraphicsPipelineクラス
class CustomReflectionGraphicsPipeline
	: public DxObject::ReflectionGraphicsPipelineState, public BaseCustomGraphicsPipeline {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	CustomReflectionGraphicsPipeline()  = default;
	~CustomReflectionGraphicsPipeline() = default;

	void RegisterBlob() override;

	void ReloadAndSetPipeline(const DirectXQueueContext* context, const D3D12_VIEWPORT& viewport, const D3D12_RECT& rect) override;
	void ReloadAndSetPipeline(const DirectXQueueContext* context, const Vector2ui& windowSize = kMainWindowSize) override;

private:

	//=========================================================================================
	// private methods
	//=========================================================================================

	void CheckAndReload() override;

};
