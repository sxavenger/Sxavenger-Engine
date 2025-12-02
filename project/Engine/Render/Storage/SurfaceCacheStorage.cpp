#include "SurfaceCacheStorage.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/SxavengerSystem.h>

////////////////////////////////////////////////////////////////////////////////////////////
// Cache structure methods
////////////////////////////////////////////////////////////////////////////////////////////

void SurfaceCacheStorage::Cache::Create(const Vector2ui& size) {

	auto device = SxavengerSystem::GetDxDevice()->GetDevice();

	{ //!< resourceの生成
		// propの設定
		D3D12_HEAP_PROPERTIES prop = {};
		prop.Type = D3D12_HEAP_TYPE_DEFAULT;

		// descの設定
		D3D12_RESOURCE_DESC desc = {};
		desc.Width            = size.x;
		desc.Height           = size.y;
		desc.MipLevels        = 1;
		desc.DepthOrArraySize = 1;
		desc.Format           = DXGI_FORMAT_R16G16B16A16_FLOAT;
		desc.SampleDesc.Count = 1;
		desc.Dimension        = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		desc.Flags            = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

		auto hr = device->CreateCommittedResource(
			&prop,
			D3D12_HEAP_FLAG_NONE,
			&desc,
			D3D12_RESOURCE_STATE_COMMON,
			nullptr,
			IID_PPV_ARGS(&resource)
		);
		DxObject::Assert(hr, L"surface cache texture create failed.");

		resource->SetName(L"Surface Cache");
	}

	{
		descriptor = SxavengerSystem::GetDescriptor(DxObject::kDescriptor_UAV);

		// descの設定
		D3D12_UNORDERED_ACCESS_VIEW_DESC desc = {};
		desc.Format        = DXGI_FORMAT_R16G16B16A16_FLOAT;
		desc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;

		device->CreateUnorderedAccessView(
			resource.Get(),
			nullptr,
			&desc,
			descriptor.GetCPUHandle()
		);
	}

}

////////////////////////////////////////////////////////////////////////////////////////////
// SurfaceCacheStorage class methods
////////////////////////////////////////////////////////////////////////////////////////////

void SurfaceCacheStorage::CreateCache(uint32_t id) {

	auto device = SxavengerSystem::GetDxDevice()->GetDevice();

	Cache cache;
	cache.Create({ 12, 12 });

}

const SurfaceCacheStorage::Cache& SurfaceCacheStorage::GetCache(uint32_t id) const {
	
}
