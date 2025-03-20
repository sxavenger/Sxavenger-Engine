#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/DirectX/DxObject/DxGraphicsPipelineState.h>
#include <Engine/System/DirectX/DirectXContext.h>

//* engine
#include <Engine/Asset/Observer/AssetObserver.h>
#include <Engine/Asset/Blob/AssetBlob.h>

////////////////////////////////////////////////////////////////////////////////////////////
// Base CustomGraphicsPipeline class
////////////////////////////////////////////////////////////////////////////////////////////
class BaseCustomGraphicsPipeline {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	BaseCustomGraphicsPipeline()          = default;
	virtual ~BaseCustomGraphicsPipeline() = default;

	//* asset option *//

	void SetAsset(const std::optional<AssetObserver<AssetBlob>>& blob, DxObject::GraphicsShaderType type);
	void CreateAsset(const std::filesystem::path& filepath, DxObject::GraphicsShaderType type);

	void ClearAsset();

	void ReloadAsset();

	virtual void RegisterBlob() = 0;

	virtual void ReloadAndSetPipeline(const DirectXThreadContext* context, const D3D12_VIEWPORT& viewport, const D3D12_RECT& rect) = 0;
	virtual void ReloadAndSetPipeline(const DirectXThreadContext* context, const Vector2ui& windowSize = kMainWindowSize) = 0;

protected:

	//=========================================================================================
	// protected variables
	//=========================================================================================

	std::array<std::optional<AssetObserver<AssetBlob>>, static_cast<uint8_t>(DxObject::GraphicsShaderType::ps) + 1> assets_;

	//=========================================================================================
	// protected methods
	//=========================================================================================

	bool CheckAsset() const;

	virtual void CheckAndReload() = 0;

};

////////////////////////////////////////////////////////////////////////////////////////////
// CustomGraphicsPipeline class
////////////////////////////////////////////////////////////////////////////////////////////
class CustomGraphicsPipeline
	: public DxObject::GraphicsPipelineState, public BaseCustomGraphicsPipeline {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	CustomGraphicsPipeline()  = default;
	~CustomGraphicsPipeline() = default;

	void RegisterBlob() override;

	void ReloadAndSetPipeline(const DirectXThreadContext* context, const D3D12_VIEWPORT& viewport, const D3D12_RECT& rect) override;
	void ReloadAndSetPipeline(const DirectXThreadContext* context, const Vector2ui& windowSize = kMainWindowSize) override;

private:

	//=========================================================================================
	// private methods
	//=========================================================================================

	void CheckAndReload() override;

};

////////////////////////////////////////////////////////////////////////////////////////////
// CustomReflectionGraphicsPipeline class
////////////////////////////////////////////////////////////////////////////////////////////
class CustomReflectionGraphicsPipeline
	: public DxObject::ReflectionGraphicsPipelineState, public BaseCustomGraphicsPipeline {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	CustomReflectionGraphicsPipeline()  = default;
	~CustomReflectionGraphicsPipeline() = default;

	void RegisterBlob() override;

	void ReloadAndSetPipeline(const DirectXThreadContext* context, const D3D12_VIEWPORT& viewport, const D3D12_RECT& rect) override;
	void ReloadAndSetPipeline(const DirectXThreadContext* context, const Vector2ui& windowSize = kMainWindowSize) override;

private:

	//=========================================================================================
	// private methods
	//=========================================================================================

	void CheckAndReload() override;

};
