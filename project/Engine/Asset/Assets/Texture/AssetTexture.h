#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* asset
#include "../BaseAsset.h"
#include "Texture.h"

//////////////////////////////////////////////////////////////////////////////////////////////
// AssetTexture class
//////////////////////////////////////////////////////////////////////////////////////////////
class AssetTexture
	: public BaseAsset {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	AssetTexture()           = default;
	~AssetTexture() override = default;

	//* async task option *//

	void Load(const DirectXThreadContext* context) override;

	AsyncExecution GetAsyncExecution() const override { return AsyncExecution::Compute; }
	//!< compute queueで実行
	// TODO: copy queueで実行するようにする

	//* inspector option *//

	void ShowInspector() override;

	//* asset option *//

	const DxObject::Descriptor& GetDescriptorSRV() const;

	const D3D12_GPU_DESCRIPTOR_HANDLE& GetGPUHandleSRV() const;

	const Texture& GetTexture() const;

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	Texture texture_;

	//=========================================================================================
	// private methods
	//=========================================================================================

	//* inspector helper methods *//

	static std::string GetEncoding(Texture::Encoding encoding);
	static std::string GetDimension(D3D12_RESOURCE_DIMENSION dimension);
	static std::string GetFormat(DXGI_FORMAT format);

	void ShowTexture();

};
