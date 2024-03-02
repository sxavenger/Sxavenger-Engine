#include "DirectXCommon.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------


////////////////////////////////////////////////////////////////////////////////////////////
// DirectXCommon class
////////////////////////////////////////////////////////////////////////////////////////////
DirectXCommon::DirectXCommon() {
}

DirectXCommon::~DirectXCommon() {
}

void DirectXCommon::Init(WinApp* winApp, int32_t clientWidth, int32_t clientHeight) {
	// DirectXObjectの初期化
	devices_ = new DxObject::Devices();
	command_ = new DxObject::Command(devices_.Get());
	descriptorHeaps_ = new DxObject::DescriptorHeaps(devices_.Get());
	swapChains_ = new DxObject::SwapChain(devices_.Get(), command_.Get(), descriptorHeaps_.Get(), winApp, clientWidth, clientHeight);
	fences_ = new DxObject::Fence(devices_.Get());

	// TODO: パイプラインの使用方法, 作成方法の修正

	depthStencil_ = new DxObject::DepthStencil(devices_.Get(), descriptorHeaps_.Get(), clientWidth, clientHeight);

	// pipeline[0]の生成
	{
		// texture pipeline
		shaderBlob_[0] = new DxObject::ShaderBlob(L"./Engine/Object3d.VS.hlsl", L"./Engine/Object3d.PS.hlsl");

		// rootSignatureDescsの初期化
		DxObject::RootSignatureDescs desc(5, 1);

		// rangeの設定
		D3D12_DESCRIPTOR_RANGE range[1] = {};
		range[0].BaseShaderRegister                = 0;
		range[0].NumDescriptors                    = 1;
		range[0].RangeType                         = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
		range[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

		// parameterの設定
		desc.param[0].ParameterType             = D3D12_ROOT_PARAMETER_TYPE_CBV;
		desc.param[0].ShaderVisibility          = D3D12_SHADER_VISIBILITY_PIXEL;
		desc.param[0].Descriptor.ShaderRegister = 0;

		desc.param[1].ParameterType             = D3D12_ROOT_PARAMETER_TYPE_CBV;
		desc.param[1].ShaderVisibility          = D3D12_SHADER_VISIBILITY_VERTEX;
		desc.param[1].Descriptor.ShaderRegister = 0;

		desc.param[2].ParameterType             = D3D12_ROOT_PARAMETER_TYPE_CBV;
		desc.param[2].ShaderVisibility          = D3D12_SHADER_VISIBILITY_PIXEL;
		desc.param[2].Descriptor.ShaderRegister = 1;

		desc.param[3].ParameterType             = D3D12_ROOT_PARAMETER_TYPE_CBV;
		desc.param[3].ShaderVisibility          = D3D12_SHADER_VISIBILITY_PIXEL;
		desc.param[3].Descriptor.ShaderRegister = 2;

		desc.param[4].ParameterType                       = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
		desc.param[4].ShaderVisibility                    = D3D12_SHADER_VISIBILITY_PIXEL;
		desc.param[4].DescriptorTable.pDescriptorRanges   = range;
		desc.param[4].DescriptorTable.NumDescriptorRanges = _countof(range);

		// samplerの設定
		desc.sampler[0].Filter           = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
		desc.sampler[0].AddressU         = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		desc.sampler[0].AddressV         = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		desc.sampler[0].AddressW         = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		desc.sampler[0].ComparisonFunc   = D3D12_COMPARISON_FUNC_NEVER;
		desc.sampler[0].MaxLOD           = D3D12_FLOAT32_MAX;
		desc.sampler[0].ShaderRegister   = 0;
		desc.sampler[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

		rootSignature_[0] = new DxObject::RootSignature(devices_.Get(), desc);
		pipeline_[0]
			= new DxObject::PipelineState(devices_.Get(), shaderBlob_[0].Get(), rootSignature_[0].Get(), clientWidth, clientHeight);
	}

	// pipeline[1]の生成
	{
		// texture pipeline
		shaderBlob_[1] = new DxObject::ShaderBlob(L"./Engine/Object3d.VS.hlsl", L"./Engine/Polygon3d.PS.hlsl");

		// rootSignatureDescsの初期化
		DxObject::RootSignatureDescs desc(4, 0);

		// parameterの設定
		desc.param[0].ParameterType             = D3D12_ROOT_PARAMETER_TYPE_CBV;
		desc.param[0].ShaderVisibility          = D3D12_SHADER_VISIBILITY_PIXEL;
		desc.param[0].Descriptor.ShaderRegister = 0;

		desc.param[1].ParameterType             = D3D12_ROOT_PARAMETER_TYPE_CBV;
		desc.param[1].ShaderVisibility          = D3D12_SHADER_VISIBILITY_VERTEX;
		desc.param[1].Descriptor.ShaderRegister = 0;

		desc.param[2].ParameterType             = D3D12_ROOT_PARAMETER_TYPE_CBV;
		desc.param[2].ShaderVisibility          = D3D12_SHADER_VISIBILITY_PIXEL;
		desc.param[2].Descriptor.ShaderRegister = 1;

		desc.param[3].ParameterType             = D3D12_ROOT_PARAMETER_TYPE_CBV;
		desc.param[3].ShaderVisibility          = D3D12_SHADER_VISIBILITY_PIXEL;
		desc.param[3].Descriptor.ShaderRegister = 2;

		rootSignature_[1] = new DxObject::RootSignature(devices_.Get(), desc);
		pipeline_[1]
			= new DxObject::PipelineState(devices_.Get(), shaderBlob_[1].Get(), rootSignature_[1].Get(), clientWidth, clientHeight);
	}
}

void DirectXCommon::Term() {
	// DirectXObjectの解放
	for (uint32_t i = 0; i < kPipelineNum_; ++i) {
		pipeline_[i].Release();
		shaderBlob_[i].Release();
		rootSignature_[i].Release();
	}

	depthStencil_.Release();
	fences_.Release();
	swapChains_.Release();
	descriptorHeaps_.Release();
	command_.Release();
	devices_.Release();
}

void DirectXCommon::BeginFrame() {
	// コマンドリストの取得
	ID3D12GraphicsCommandList* commandList = command_->GetCommandList();

	// 書き込みバックバッファのインデックスを取得
	backBufferIndex_ = swapChains_->GetSwapChain()->GetCurrentBackBufferIndex();

	commandList->ResourceBarrier(
		1,
		swapChains_->GetTransitionBarrier(backBufferIndex_, D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET)
	);

	D3D12_CPU_DESCRIPTOR_HANDLE handle_RTV = swapChains_->GetHandleCPU_RTV(backBufferIndex_); 

	commandList->OMSetRenderTargets(
		1,
		&handle_RTV,
		false,
		&depthStencil_->GetHandle()
	);

	// 画面のクリア
	float clearColor[] = { 0.1f, 0.25f, 0.5f, 1.0f };
	commandList->ClearRenderTargetView(
		handle_RTV,
		clearColor,
		0, nullptr
	);

	// 深度をクリア
	commandList->ClearDepthStencilView(
		depthStencil_->GetHandle(),
		D3D12_CLEAR_FLAG_DEPTH,
		1.0f,
		0, 0, nullptr
	);
}

void DirectXCommon::EndFrame() {
	command_->GetCommandList()->ResourceBarrier(
		1,
		swapChains_->GetTransitionBarrier(backBufferIndex_, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT)
	);

	command_->Close();

	swapChains_->Present(1, 0);

	// GPUにシグナルを送る
	fences_->AddFenceValue();

	command_->Signal(fences_.Get());

	fences_->WaitGPU();

	command_->Reset();
}

void DirectXCommon::SetPipelineState(uint32_t index) {
	pipeline_[index]->SetCommandPipelineState(command_.Get());
}

DirectXCommon* DirectXCommon::GetInstance() {
	static DirectXCommon instance;
	return &instance;
}
