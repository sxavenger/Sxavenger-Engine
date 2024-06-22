#include <DxCSDimensionBuffer.h>

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include <MyEngine.h>

////////////////////////////////////////////////////////////////////////////////////////////
// CSBaseDimensionBuffer class methods
////////////////////////////////////////////////////////////////////////////////////////////

void DxObject::CSBaseDimensionBuffer::Init(
	DXGI_FORMAT format, DimensionType dimensionType, const DimensionSize& dimensionSize) {

	// デバイスの取り出し
	auto device = MyEngine::GetDevice();

	// 引数の保存
	format_        = format;
	dimensionSize_ = dimensionSize;
	dimensionType_ = dimensionType;

	// dimensionSizeの調整
	if (dimensionType_ <= DimensionType::DIMENSION_TEXTURE2D) {
		dimensionSize_.depth = 1; //!< texture2D以下では使われない

		if (dimensionType_ <= DimensionType::DIMENSION_TEXTURE1D) {
			dimensionSize_.height = 1; //!< texture1D以下では使われない
		}
	}

	// resourceの生成
	uavResource_ = DxMethods::CreateDimensionResource(
		device,
		D3D12_HEAP_TYPE_DEFAULT,
		format, dimensionType_, dimensionSize_,
		D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS,
		D3D12_RESOURCE_STATE_COMMON
	);
	
	// descriptorの取得
	descriptorUAV_ = MyEngine::GetCurrentDescripor(UAV);
	descriptorSRV_ = MyEngine::GetCurrentDescripor(SRV);

	// viewの生成
	DxMethods::CreateDimensionUnorderedAccessView(
		device,
		format_, dimensionType_,
		uavResource_.Get(), descriptorUAV_
	);

	DxMethods::CreateDimensionShaderResourceView(
		device,
		format_, dimensionType_,
		uavResource_.Get(), descriptorSRV_
	);

}

////////////////////////////////////////////////////////////////////////////////////////////
// DxMethods namespace methods
////////////////////////////////////////////////////////////////////////////////////////////

ComPtr<ID3D12Resource> DxMethods::CreateDimensionResource(
	ID3D12Device* device,
	D3D12_HEAP_TYPE heapType,
	DXGI_FORMAT format, DxObject::DimensionType dimensionType, const DxObject::DimensionSize& dimensionSize,
	D3D12_RESOURCE_FLAGS flags, D3D12_RESOURCE_STATES state) {
	
	ComPtr<ID3D12Resource> result;

	D3D12_HEAP_PROPERTIES prop = {};
	prop.Type = heapType;

	D3D12_RESOURCE_DESC desc = {};
	desc.Dimension        = static_cast<D3D12_RESOURCE_DIMENSION>(dimensionType);
	desc.MipLevels        = 1;
	desc.SampleDesc.Count = 1;
	desc.Format           = format;
	desc.Flags            = flags;

	// sizeの指定
	desc.Width            = dimensionSize.width;
	desc.Height           = 1;
	desc.DepthOrArraySize = 1;

	if (dimensionType >= DxObject::DimensionType::DIMENSION_TEXTURE2D) {
		desc.Height = dimensionSize.height;

		if (dimensionType >= DxObject::DimensionType::DIMENSION_TEXTURE3D) {
			desc.DepthOrArraySize = dimensionSize.depth;
		}
	}
	

	auto hr = device->CreateCommittedResource(
		&prop,
		D3D12_HEAP_FLAG_NONE,
		&desc,
		state,
		nullptr,
		IID_PPV_ARGS(&result)
	);

	assert(SUCCEEDED(hr));

	return result;
}

void DxMethods::CreateDimensionUnorderedAccessView(
	ID3D12Device* device,
	DXGI_FORMAT format, DxObject::DimensionType dimensionType,
	ID3D12Resource* resource, const DxObject::Descriptor& descriptor) {

	// desc設定
	D3D12_UNORDERED_ACCESS_VIEW_DESC desc = {};
	desc.Format = format;

	//!< それぞれ設定が違うのでcase分けして設定
	switch (dimensionType) {
		case DxObject::DimensionType::DIMENSION_TEXTURE1D:

			desc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE1D;

			break;

		case DxObject::DimensionType::DIMENSION_TEXTURE2D:

			desc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;

			break;

		case DxObject::DimensionType::DIMENSION_TEXTURE3D:

			desc.ViewDimension    = D3D12_UAV_DIMENSION_TEXTURE3D;
			desc.Texture3D.WSize  = UINT_MAX;

			break;
	}

	device->CreateUnorderedAccessView(
		resource, nullptr, &desc, descriptor.handleCPU
	);
}

void DxMethods::CreateDimensionShaderResourceView(
	ID3D12Device* device,
	DXGI_FORMAT format, DxObject::DimensionType dimensionType,
	ID3D12Resource* resource, const DxObject::Descriptor& descriptor) {

	// desc設定
	D3D12_SHADER_RESOURCE_VIEW_DESC desc = {};
	desc.Format                  = format;
	desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

	//!< それぞれ設定が違うのでcase分けして設定
	switch (dimensionType) {
		case DxObject::DimensionType::DIMENSION_TEXTURE1D:

			desc.ViewDimension       = D3D12_SRV_DIMENSION_TEXTURE1D;
			desc.Texture1D.MipLevels = 1;

			break;

		case DxObject::DimensionType::DIMENSION_TEXTURE2D:

			desc.ViewDimension       = D3D12_SRV_DIMENSION_TEXTURE2D;
			desc.Texture2D.MipLevels = 1;

			break;

		case DxObject::DimensionType::DIMENSION_TEXTURE3D:

			desc.ViewDimension       = D3D12_SRV_DIMENSION_TEXTURE3D;
			desc.Texture3D.MipLevels = 1;

			break;
	}

	device->CreateShaderResourceView(
		resource, &desc, descriptor.handleCPU
	);
}
