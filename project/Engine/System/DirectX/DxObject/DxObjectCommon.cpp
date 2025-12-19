#include "DxObjectCommon.h"
SXAVENGER_ENGINE_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/Utility/StreamLogger.h>

//* windows
#include <comdef.h>

////////////////////////////////////////////////////////////////////////////////////////////
// methods
////////////////////////////////////////////////////////////////////////////////////////////

ComPtr<ID3D12Resource> DXOBJECT CreateBufferResource(
	ID3D12Device* device,
	D3D12_HEAP_TYPE heapType,
	size_t sizeInBytes,
	D3D12_RESOURCE_FLAGS flags, D3D12_RESOURCE_STATES state) {

	StreamLogger::AssertA(sizeInBytes != 0);

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

ComPtr<ID3D12Resource> DXOBJECT CreateBufferResource(
	ID3D12Device* device, size_t sizeInBytes) {

	StreamLogger::AssertA(sizeInBytes != 0);

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

UINT DXOBJECT RoundUp(UINT round, UINT thread) {
	StreamLogger::AssertA(thread > 0, "round up thread is zero.");
	return (round + thread - 1) / thread;
}

Vector3ui DXOBJECT RoundUp(const Vector3ui& round, const Vector3ui& thread) {
	return { RoundUp(round.x, thread.x), RoundUp(round.y, thread.y), RoundUp(round.z, thread.z) };
}

DXOBJECT CompileProfile DXOBJECT ToProfile(GraphicsShaderType type) {
	switch (type) {
		case GraphicsShaderType::vs:
			return CompileProfile::vs;

		case GraphicsShaderType::gs:
			return CompileProfile::gs;

		case GraphicsShaderType::ms:
			return CompileProfile::ms;
			
		case GraphicsShaderType::as:
			return CompileProfile::as;

		case GraphicsShaderType::ps:
			return CompileProfile::ps;
	}

	StreamLogger::Exception("Graphics Shader Type is undefine.");
}

void DXOBJECT Assert(HRESULT hr, const std::wstring& label, const std::source_location& location) {
	StreamLogger::AssertW(SUCCEEDED(hr), label, std::format(L"_com_error: {}", _com_error(hr).ErrorMessage()), location);
}
