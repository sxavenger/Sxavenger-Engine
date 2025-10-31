#include "DxObjectCommon.h"
_DXOBJECT_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* windows
#include <comdef.h>

////////////////////////////////////////////////////////////////////////////////////////////
// methods
////////////////////////////////////////////////////////////////////////////////////////////

ComPtr<ID3D12Resource> _DXOBJECT CreateBufferResource(
	ID3D12Device* device,
	D3D12_HEAP_TYPE heapType,
	size_t sizeInBytes,
	D3D12_RESOURCE_FLAGS flags, D3D12_RESOURCE_STATES state) {

	Exception::Assert(sizeInBytes != 0);

	ComPtr<ID3D12Resource> result;

	D3D12_HEAP_PROPERTIES prop = {};
	prop.Type = heapType;

	D3D12_RESOURCE_DESC desc = {};
	desc.Dimension        = D3D12_RESOURCE_DIMENSION_BUFFER;
	desc.Width            = sizeInBytes;
	desc.Height           = 1;
	desc.DepthOrArraySize = 1;
	desc.MipLevels        = 1;
	desc.SampleDesc.Count = 1;
	desc.Layout           = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	desc.Flags            = flags;

	auto hr = device->CreateCommittedResource(
		&prop,
		D3D12_HEAP_FLAG_NONE,
		&desc,
		state,
		nullptr,
		IID_PPV_ARGS(result.GetAddressOf())
	);
	DxObject::Assert(hr, L"resource create failed.");

	return result;
}

ComPtr<ID3D12Resource> _DXOBJECT CreateBufferResource(
	ID3D12Device* device, size_t sizeInBytes) {

	Exception::Assert(sizeInBytes != 0);

	ComPtr<ID3D12Resource> result;

	// 頂点リソース用のヒープの設定
	D3D12_HEAP_PROPERTIES prop = {};
	prop.Type = D3D12_HEAP_TYPE_UPLOAD;

	// 頂点リソースの設定
	D3D12_RESOURCE_DESC desc = {};
	desc.Dimension        = D3D12_RESOURCE_DIMENSION_BUFFER;
	desc.Width            = sizeInBytes;
	desc.Height           = 1;
	desc.DepthOrArraySize = 1;
	desc.MipLevels        = 1;
	desc.SampleDesc.Count = 1;
	desc.Layout           = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	auto hr = device->CreateCommittedResource(
		&prop,
		D3D12_HEAP_FLAG_NONE,
		&desc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(result.GetAddressOf())
	);
	DxObject::Assert(hr, L"resource create failed.");

	return result;

}

UINT _DXOBJECT RoundUp(UINT round, UINT thread) {
	Exception::Assert(thread > 0);
	return (round + thread - 1) / thread;
}

Vector3ui _DXOBJECT RoundUp(const Vector3ui& round, const Vector3ui& thread) {
	return { RoundUp(round.x, thread.x), RoundUp(round.y, thread.y), RoundUp(round.z, thread.z) };
}

CompileProfile _DXOBJECT ToProfile(GraphicsShaderType type) {
	switch (type) {
		case GraphicsShaderType::vs:
			return CompileProfile::vs;
			break;

		case GraphicsShaderType::gs:
			return CompileProfile::gs;
			break;

		case GraphicsShaderType::ms:
			return CompileProfile::ms;
			break;
		case GraphicsShaderType::as:
			return CompileProfile::as;
			break;

		case GraphicsShaderType::ps:
			return CompileProfile::ps;
			break;
	}

	Exception::Assert(false, "Graphics Shader Type is undefine.");
	return {};
}

void _DXOBJECT Assert(HRESULT hr, const std::wstring& label, const std::source_location& location) {
	Exception::AssertW(SUCCEEDED(hr), label, std::format(L"_com_error: {}", _com_error(hr).ErrorMessage()), location);
}
