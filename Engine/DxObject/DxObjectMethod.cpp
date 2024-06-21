#include <DxObjectMethod.h>

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include <Logger.h>

////////////////////////////////////////////////////////////////////////////////////////////
// DxObjectMethod
////////////////////////////////////////////////////////////////////////////////////////////

ComPtr<ID3D12DescriptorHeap> DxObjectMethod::CreateDescriptorHeap(
	ID3D12Device* device, D3D12_DESCRIPTOR_HEAP_TYPE heapType, UINT numDescriptors, bool shaderVisible) {

	ComPtr<ID3D12DescriptorHeap> result;

	D3D12_DESCRIPTOR_HEAP_DESC desc = {};
	desc.Type           = heapType;
	desc.NumDescriptors = numDescriptors;
	desc.Flags          = shaderVisible
		? D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE : D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

	auto hr = device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&result));
	assert(SUCCEEDED(hr));

	return result;
}

ComPtr<IDxcBlob> DxObjectMethod::CompileShader(
	const std::wstring& filePath,
	const wchar_t* profile,
	IDxcUtils* dxcUtils,
	IDxcCompiler3* dxcCompiler,
	IDxcIncludeHandler* includeHandler) {

	// hlslファイルを読み込む
	ComPtr<IDxcBlobEncoding> shaderSource;
	auto hr = dxcUtils->LoadFile(filePath.c_str(), nullptr, &shaderSource);

	// 読めなかったら止める
	if (FAILED(hr)) {
		std::wstring outputLog = L"Error : HLSL Not Found. filePath: ";
		Log(outputLog + filePath);

		std::string	errorLog;
		errorLog
			= "[HLSL Not Found] \n filePath: " + ToString(filePath);

		Assert(false, errorLog, "Error: CompileShader");
	}

	// 読み込んだファイルの内容を設定する
	DxcBuffer shaderSourceBuffer;
	shaderSourceBuffer.Ptr      = shaderSource->GetBufferPointer();
	shaderSourceBuffer.Size     = shaderSource->GetBufferSize();
	shaderSourceBuffer.Encoding = DXC_CP_UTF8;

	LPCWSTR arguments[] = {
		filePath.c_str(),         //!< コンパイル対象のhlslファイルパス
		L"-E", L"main",           //!< エントリーポイントの指定
		L"-T", profile,           //!< ShaderProfileの設定
		L"-Zi", L"-Qembed_debug", //!< デバッグ用情報を埋め込む
		L"-Od",                   //!< 最適化を外しておく
		L"-Zpr"                   //!< メモリレイアウトは行優先
	};

	// ShaderCompile
	ComPtr<IDxcResult> shaderResult;
	hr = dxcCompiler->Compile(
		&shaderSourceBuffer,
		arguments,
		_countof(arguments),
		includeHandler,
		IID_PPV_ARGS(&shaderResult)
	);

	assert(SUCCEEDED(hr));

	// 警告エラーだった場合, プログラムの停止
	ComPtr<IDxcBlobUtf8> shaderError;
	hr = shaderResult->GetOutput(DXC_OUT_ERRORS, IID_PPV_ARGS(&shaderError), nullptr);

	if (shaderError != nullptr && shaderError->GetStringLength() != 0) {

		Log("//-----------------------------------------------------------------------------------------");
		Log(shaderError->GetStringPointer()); //!< shaderの構文error
		Log("//-----------------------------------------------------------------------------------------");

		// hlslのerrorをwindowで出力
		std::string	errorLog;
		errorLog
			= "[HLSL Error] \n filePath: " + ToString(filePath);

		Assert(false, errorLog, "Error: CompileShader");
	}

	IDxcBlob* shaderBlob = nullptr;
	hr = shaderResult->GetOutput(DXC_OUT_OBJECT, IID_PPV_ARGS(&shaderBlob), nullptr);
	assert(SUCCEEDED(hr));

	return shaderBlob;
}

ComPtr<IDxcBlob> DxObjectMethod::CompileShader(
	const std::wstring& filePath,
	const std::wstring& entryPoint,
	const wchar_t* profile,
	IDxcUtils* dxcUtils,
	IDxcCompiler3* dxcCompiler,
	IDxcIncludeHandler* includeHandler) {


	// hlslファイルを読み込む
	ComPtr<IDxcBlobEncoding> shaderSource;
	auto hr = dxcUtils->LoadFile(filePath.c_str(), nullptr, &shaderSource);

	// 読めなかったら止める
	if (FAILED(hr)) {
		std::wstring outputLog = L"Error : HLSL Not Found. filePath: ";
		Log(outputLog + filePath);

		std::string	errorLog;
		errorLog
			= "[HLSL Not Found] \n filePath: " + ToString(filePath);

		Assert(false, errorLog, "Error: CompileShader");
	}

	// 読み込んだファイルの内容を設定する
	DxcBuffer shaderSourceBuffer;
	shaderSourceBuffer.Ptr      = shaderSource->GetBufferPointer();
	shaderSourceBuffer.Size     = shaderSource->GetBufferSize();
	shaderSourceBuffer.Encoding = DXC_CP_UTF8;

	LPCWSTR arguments[] = {
		filePath.c_str(),          //!< コンパイル対象のhlslファイルパス
		L"-E", entryPoint.c_str(), //!< エントリーポイントの指定
		L"-T", profile,            //!< ShaderProfileの設定
		L"-Zi", L"-Qembed_debug",  //!< デバッグ用情報を埋め込む
		L"-Od",                    //!< 最適化を外しておく
		L"-Zpr"                    //!< メモリレイアウトは行優先
	};

	// ShaderCompile
	ComPtr<IDxcResult> shaderResult;
	hr = dxcCompiler->Compile(
		&shaderSourceBuffer,
		arguments,
		_countof(arguments),
		includeHandler,
		IID_PPV_ARGS(&shaderResult)
	);

	assert(SUCCEEDED(hr));

	// 警告エラーだった場合, プログラムの停止
	ComPtr<IDxcBlobUtf8> shaderError;
	hr = shaderResult->GetOutput(DXC_OUT_ERRORS, IID_PPV_ARGS(&shaderError), nullptr);

	if (shaderError != nullptr && shaderError->GetStringLength() != 0) {

		Log("//-----------------------------------------------------------------------------------------");
		Log(shaderError->GetStringPointer()); //!< shaderの構文error
		Log("//-----------------------------------------------------------------------------------------");

		// hlslのerrorをwindowで出力
		std::string	errorLog;
		errorLog
			= "[HLSL Error] \n filePath: " + ToString(filePath);

		Assert(false, errorLog, "Error: CompileShader");
	}

	IDxcBlob* shaderBlob = nullptr;
	hr = shaderResult->GetOutput(DXC_OUT_OBJECT, IID_PPV_ARGS(&shaderBlob), nullptr);
	assert(SUCCEEDED(hr));

	return shaderBlob;
}

ComPtr<ID3D12Resource> DxObjectMethod::CreateBufferResource(
	ID3D12Device* device, size_t sizeInBytes) {

	assert(sizeInBytes != 0);

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
		IID_PPV_ARGS(&result)
	);

	assert(SUCCEEDED(hr));

	return result;
}

ComPtr<ID3D12RootSignature> DxObjectMethod::CreateRootSignature(
	ID3D12Device* device, const D3D12_ROOT_SIGNATURE_DESC& desc) {
	
	ComPtr<ID3D12RootSignature> result;

	ComPtr<ID3DBlob> signatureBlob;
	ComPtr<ID3DBlob> signatureErrorBlob;

	auto hr = D3D12SerializeRootSignature(
		&desc,
		D3D_ROOT_SIGNATURE_VERSION_1,
		&signatureBlob,
		&signatureErrorBlob
	);

	if (FAILED(hr)) {
		Log("//-----------------------------------------------------------------------------------------");
		Log("Failed: Create RootSignatuer");
		Log(reinterpret_cast<char*>(signatureErrorBlob->GetBufferPointer()));
		Log("//-----------------------------------------------------------------------------------------");
		assert(false);
	}

	hr = device->CreateRootSignature(
		0,
		signatureBlob->GetBufferPointer(),
		signatureBlob->GetBufferSize(),
		IID_PPV_ARGS(&result)
	);

	assert(SUCCEEDED(hr));

	return result;
}

ComPtr<ID3D12Resource> DxObjectMethod::CreateDepthStencilTextureResource(
	ID3D12Device* device, int32_t width, int32_t height) {

	// descの設定
	D3D12_RESOURCE_DESC desc = {};
	desc.Width            = width;
	desc.Height           = height;
	desc.MipLevels        = 1;
	desc.DepthOrArraySize = 1;
	desc.Format           = DXGI_FORMAT_D24_UNORM_S8_UINT;
	desc.SampleDesc.Count = 1;
	desc.Dimension        = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	desc.Flags            = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

	// propの設定
	D3D12_HEAP_PROPERTIES prop = {};
	prop.Type = D3D12_HEAP_TYPE_DEFAULT;

	// 深度値のクリア設定
	D3D12_CLEAR_VALUE clear = {};
	clear.DepthStencil.Depth = 1.0f;
	clear.Format             = desc.Format;

	// resourceの設定
	ID3D12Resource* result;

	auto hr = device->CreateCommittedResource(
		&prop,
		D3D12_HEAP_FLAG_NONE,
		&desc,
		D3D12_RESOURCE_STATE_DEPTH_WRITE,
		&clear,
		IID_PPV_ARGS(&result)
	);

	assert(SUCCEEDED(hr));

	return result;
}

D3D12_CPU_DESCRIPTOR_HANDLE DxObjectMethod::GetCPUDescriptorHandle(
	ID3D12DescriptorHeap* descriptorHeap,
	uint32_t descriptorSize,
	uint32_t index) {

	D3D12_CPU_DESCRIPTOR_HANDLE result = descriptorHeap->GetCPUDescriptorHandleForHeapStart();
	result.ptr += (descriptorSize * index);
	return result;
}

D3D12_GPU_DESCRIPTOR_HANDLE DxObjectMethod::GetGPUDescriptorHandle(
	ID3D12DescriptorHeap* descriptorHeap,
	uint32_t descriptorSize,
	uint32_t index) {

	D3D12_GPU_DESCRIPTOR_HANDLE result = descriptorHeap->GetGPUDescriptorHandleForHeapStart();
	result.ptr += (descriptorSize * index);
	return result;
}