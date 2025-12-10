#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* DirectX12
#include <d3d12.h>
#include <dxgi1_6.h>

//* c++
#include <cstdint>
#include <variant>

//* engine
#include <Engine/System/Utility/ComPtr.h>
#include <Engine/System/Utility/Logger.h>

//* lib
#include <Lib/Geometry/Vector3.h>

//-----------------------------------------------------------------------------------------
// comment
//-----------------------------------------------------------------------------------------
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")

////////////////////////////////////////////////////////////////////////////////////////////
// DXOBJECT DEFINE
////////////////////////////////////////////////////////////////////////////////////////////

#define DXOBJECT_NAMESPACE_BEGIN namespace DxObject {
#define DXOBJECT_NAMESPACE_END   }
#define DXOBJECT                 DxObject::
#define DXOBJECT_USING           using namespace DxObject;

////////////////////////////////////////////////////////////////////////////////////////////
// DXOBJECT Common
////////////////////////////////////////////////////////////////////////////////////////////
DXOBJECT_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////////////
// using
////////////////////////////////////////////////////////////////////////////////////////////
using GPUBuffer = std::variant<D3D12_GPU_VIRTUAL_ADDRESS, D3D12_GPU_DESCRIPTOR_HANDLE>;

////////////////////////////////////////////////////////////////////////////////////////////
// ShaderVisibility enum class
////////////////////////////////////////////////////////////////////////////////////////////
enum class ShaderVisibility {
	VISIBILITY_ALL           = D3D12_SHADER_VISIBILITY_ALL,
	VISIBILITY_VERTEX        = D3D12_SHADER_VISIBILITY_VERTEX,
	VISIBILITY_HULL          = D3D12_SHADER_VISIBILITY_HULL,
	VISIBILITY_DOMAIN        = D3D12_SHADER_VISIBILITY_DOMAIN,
	VISIBILITY_GEOMETRY      = D3D12_SHADER_VISIBILITY_GEOMETRY,
	VISIBILITY_PIXEL         = D3D12_SHADER_VISIBILITY_PIXEL,
	VISIBILITY_AMPLIFICATION = D3D12_SHADER_VISIBILITY_AMPLIFICATION,
	VISIBILITY_MESH          = D3D12_SHADER_VISIBILITY_MESH
};

////////////////////////////////////////////////////////////////////////////////////////////
// SamplerMode enum class
////////////////////////////////////////////////////////////////////////////////////////////
enum SamplerMode {
	MODE_WRAP        = D3D12_TEXTURE_ADDRESS_MODE_WRAP,
	MODE_MIRROR      = D3D12_TEXTURE_ADDRESS_MODE_MIRROR,
	MODE_CLAMP       = D3D12_TEXTURE_ADDRESS_MODE_CLAMP,
	MODE_BORDER      = D3D12_TEXTURE_ADDRESS_MODE_BORDER,
	MODE_MIRROR_ONCE = D3D12_TEXTURE_ADDRESS_MODE_MIRROR_ONCE
};

////////////////////////////////////////////////////////////////////////////////////////////
// DescriptorType enum
////////////////////////////////////////////////////////////////////////////////////////////
enum DescriptorType : uint32_t {
	kDescriptor_RTV,
	kDescriptor_DSV,
	kDescriptor_CBV_SRV_UAV,
	kCountOfDescriptorTypeCount,

	kDescriptor_SRV = kDescriptor_CBV_SRV_UAV,
	kDescriptor_UAV = kDescriptor_CBV_SRV_UAV,
};

////////////////////////////////////////////////////////////////////////////////////////////
// CompileProfile enum class
////////////////////////////////////////////////////////////////////////////////////////////
enum class CompileProfile : uint8_t {
	vs,
	gs,
	ms,
	as,
	ps,
	cs,
	lib,
};

////////////////////////////////////////////////////////////////////////////////////////////
// GraphicsShaderType enum class
////////////////////////////////////////////////////////////////////////////////////////////
enum class GraphicsShaderType : uint8_t {
	vs,
	gs,
	ms,
	as,
	ps
};

////////////////////////////////////////////////////////////////////////////////////////////
// common methods
////////////////////////////////////////////////////////////////////////////////////////////

constexpr DXGI_FORMAT ConvertToSRGB(DXGI_FORMAT format) {
	switch (format) {
		case DXGI_FORMAT_R8G8B8A8_UNORM:
			return DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;

		case DXGI_FORMAT_BC1_UNORM:
			return DXGI_FORMAT_BC1_UNORM_SRGB;

		case DXGI_FORMAT_BC2_UNORM:
			return DXGI_FORMAT_BC2_UNORM_SRGB;

		case DXGI_FORMAT_BC3_UNORM:
			return DXGI_FORMAT_BC3_UNORM_SRGB;

		case DXGI_FORMAT_B8G8R8A8_UNORM:
			return DXGI_FORMAT_B8G8R8A8_UNORM_SRGB;

		case DXGI_FORMAT_B8G8R8X8_UNORM:
			return DXGI_FORMAT_B8G8R8X8_UNORM_SRGB;

		case DXGI_FORMAT_BC7_UNORM:
			return DXGI_FORMAT_BC7_UNORM_SRGB;

		default:
			return format;
	}
}

NODISCARD ComPtr<ID3D12Resource> CreateBufferResource(
	ID3D12Device* device,
	D3D12_HEAP_TYPE heapType,
	size_t sizeInBytes,
	D3D12_RESOURCE_FLAGS flags, D3D12_RESOURCE_STATES state
);

NODISCARD ComPtr<ID3D12Resource> CreateBufferResource(
	ID3D12Device* device,
	size_t sizeInBytes
);

UINT RoundUp(UINT round, UINT thread);
Vector3ui RoundUp(const Vector3ui& round, const Vector3ui& thread);

CompileProfile ToProfile(GraphicsShaderType type);

void Assert(HRESULT hr, const std::wstring& label, const std::source_location& location = std::source_location::current());

////////////////////////////////////////////////////////////////////////////////////////////
// Configs
////////////////////////////////////////////////////////////////////////////////////////////

constexpr const D3D_SHADER_MODEL kHeighestShaderModel = D3D_SHADER_MODEL_6_6;
static_assert(kHeighestShaderModel >= D3D_SHADER_MODEL_6_5, "mesh shader is 6.5 or higher");

constexpr const DXGI_FORMAT kDefaultScreenFormat     = DXGI_FORMAT_R8G8B8A8_UNORM;          //!< スクリーン画面のformat
constexpr const DXGI_FORMAT kDefaultScreenViewFormat = ConvertToSRGB(kDefaultScreenFormat); //!< 最終的なスクリーン画面のformat
constexpr const DXGI_FORMAT kDefaultOffscreenFormat  = kDefaultScreenFormat;                //!< offscreenで使われるformat

constexpr const DXGI_FORMAT kDefaultDepthFormat     = DXGI_FORMAT_D32_FLOAT_S8X24_UINT;
constexpr const DXGI_FORMAT kDefaultDepthViewFormat = DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS;

DXOBJECT_NAMESPACE_END
