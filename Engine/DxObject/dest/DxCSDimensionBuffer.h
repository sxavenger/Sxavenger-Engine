#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
// directX
#include <d3d12.h>
#include <dxgi1_6.h>

// c++
#include <cstdint>
#include <cassert>
#include <vector>
#include <memory>

// DxObject
#include <DxObjectMethod.h>

// ComPtr
#include <ComPtr.h>

// Geometry
#include <Vector3.h>

//-----------------------------------------------------------------------------------------
// comment
//-----------------------------------------------------------------------------------------
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")

////////////////////////////////////////////////////////////////////////////////////////////
// DxObject namespace
////////////////////////////////////////////////////////////////////////////////////////////
namespace DxObject { //!< DxSource, use Compute Shader

	////////////////////////////////////////////////////////////////////////////////////////////
	// DimensionType enum
	////////////////////////////////////////////////////////////////////////////////////////////
	enum DimensionType {
		DIMENSION_TEXTURE1D = D3D12_RESOURCE_DIMENSION_TEXTURE1D, //!< x
		DIMENSION_TEXTURE2D = D3D12_RESOURCE_DIMENSION_TEXTURE2D, //!< xy
		DIMENSION_TEXTURE3D = D3D12_RESOURCE_DIMENSION_TEXTURE3D, //!< xyz
	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// DimensionSize structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct DimensionSize {
		UINT64 width;
		UINT   height; //!< max: 4294967295
		UINT16 depth;  //<! max: 65535
	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// CSBaseDimensionBuffer class
	////////////////////////////////////////////////////////////////////////////////////////////
	class CSBaseDimensionBuffer {
	public:

		//=========================================================================================
		// public methods
		//=========================================================================================

		CSBaseDimensionBuffer() = default;

		void Init(DXGI_FORMAT format, DimensionType dimensionType, const DimensionSize& dimensionSize);

	private:

		//=========================================================================================
		// private variables
		//=========================================================================================

		/* resources */
		ComPtr<ID3D12Resource> uavResource_;
		/*ComPtr<ID3D12Resource> writeResource_;*/

		/* descriptors */
		DxObject::Descriptor descriptorUAV_;
		DxObject::Descriptor descriptorSRV_;

		/* format */
		DXGI_FORMAT format_;

		/* dimension */
		DimensionSize dimensionSize_;
		DimensionType dimensionType_;

	};

}

////////////////////////////////////////////////////////////////////////////////////////////
// DxMethods namespace
////////////////////////////////////////////////////////////////////////////////////////////
namespace DxMethods {

	ComPtr<ID3D12Resource> CreateDimensionResource(
		ID3D12Device* device,
		D3D12_HEAP_TYPE heapType,
		DXGI_FORMAT format, DxObject::DimensionType dimensionType, const DxObject::DimensionSize& dimensionSize,
		D3D12_RESOURCE_FLAGS flags, D3D12_RESOURCE_STATES state
	);

	void CreateDimensionUnorderedAccessView(
		ID3D12Device* device,
		DXGI_FORMAT format, DxObject::DimensionType dimensionType,
		ID3D12Resource* resource, const DxObject::Descriptor& descriptor
	);

	void CreateDimensionShaderResourceView(
		ID3D12Device* device,
		DXGI_FORMAT format, DxObject::DimensionType dimensionType,
		ID3D12Resource* resource, const DxObject::Descriptor& descriptor
	);
}