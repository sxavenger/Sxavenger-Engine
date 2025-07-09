#include "FeatureLevelGameLoop.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/Utility/Logger.h>

//* c++
#include <format>

////////////////////////////////////////////////////////////////////////////////////////////
// FeatureLevelGameLoop class methods
////////////////////////////////////////////////////////////////////////////////////////////

void FeatureLevelGameLoop::Init(GameLoop::Context* context) {
	context->SetProcess(GameLoop::Process::Init, std::nullopt, [this]() {
		file_.open(filepath_);
		CheckFeatureLevel();
		file_.close();
	});
}

void FeatureLevelGameLoop::Term() {
}

void FeatureLevelGameLoop::CheckFeatureLevel() {

	if (!CheckFactory()) {
		return;
	}

	if (!CheckAdapter()) {
		return;
	}

	if (!CheckDevice()) {
		return;
	}

	CheckSupport();
}

bool FeatureLevelGameLoop::CheckFactory() {

	auto hr = CreateDXGIFactory(IID_PPV_ARGS(&factory_));
	file_ << "[IDXGIFactory7] CreateDXGIFactory(...): " << (SUCCEEDED(hr) ? "true" : "false") << std::endl;

	if (FAILED(hr)) {
		return false;
	}

	BOOL isTearingSupport = false;
	hr = factory_->CheckFeatureSupport(DXGI_FEATURE_PRESENT_ALLOW_TEARING, &isTearingSupport, sizeof(BOOL));

	file_ << "[IDXGIFactory7] CheckFeatureSupport(DXGI_FEATURE_PRESENT_ALLOW_TEARING) = " << (isTearingSupport ? "true" : "false") << std::endl;

	return true;
}

bool FeatureLevelGameLoop::CheckAdapter() {

	for (UINT i = 0;
		factory_->EnumAdapterByGpuPreference(i, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&adapter_)) != DXGI_ERROR_NOT_FOUND;
		++i) {

		// アダプタ情報を取得
		DXGI_ADAPTER_DESC3 desc = {};
		auto hr = adapter_->GetDesc3(&desc);

		if (FAILED(hr)) {
			file_ << "[IDXGIAdapter4] GetDesc failed." << std::endl;
			continue;
		}

		if (!(desc.Flags & DXGI_ADAPTER_FLAG3_SOFTWARE)) {
			file_ << "[IDXGIAdapter4] Adapter Name: " << ToString(desc.Description) << std::endl;
			return true;
		}
	}

	file_ << "[IDXGIAdapter4] Not found Hardware Adapter." << std::endl;
	return false;
}

bool FeatureLevelGameLoop::CheckDevice() {

	static const std::pair<D3D_FEATURE_LEVEL, LPCSTR> featureLevels[] = {
		{ D3D_FEATURE_LEVEL_12_2, "12_2" },
		{ D3D_FEATURE_LEVEL_12_1, "12_1" },
		{ D3D_FEATURE_LEVEL_12_0, "12_0" },
	};

	for (size_t i = 0; i < _countof(featureLevels); ++i) {
		auto hr = D3D12CreateDevice(adapter_.Get(), featureLevels[i].first, IID_PPV_ARGS(&device_));
		if (SUCCEEDED(hr)) {
			file_ << "[ID3D12Device8] feature level: " << featureLevels[i].second << std::endl;
			return true;
		}
	}

	file_ << "[ID3D12Device8] feature level D3D_FEATURE_LEVEL_12_X not support." << std::endl;
	return false;
}

void FeatureLevelGameLoop::CheckSupport() {

	
	{ //!< shader model

		for (uint32_t model = D3D_HIGHEST_SHADER_MODEL; model >= D3D_SHADER_MODEL_5_1; --model) {
			D3D12_FEATURE_DATA_SHADER_MODEL shaderModel = { D3D_HIGHEST_SHADER_MODEL };
			auto hr = device_->CheckFeatureSupport(D3D12_FEATURE_SHADER_MODEL, &shaderModel, sizeof(D3D12_FEATURE_DATA_SHADER_MODEL));

			if (SUCCEEDED(hr)) {
				file_ << std::format("[Feature Support::D3D12_FEATURE_SHADER_MODEL]: 0x{:x}", static_cast<uint32_t>(shaderModel.HighestShaderModel)) << std::endl;
				break;
			}
		}
	}

	{ //!< option5 (raytracing)

		D3D12_FEATURE_DATA_D3D12_OPTIONS5 option = {};
		auto hr = device_->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS5, &option, sizeof(D3D12_FEATURE_DATA_D3D12_OPTIONS5));

		if (SUCCEEDED(hr) && option.RaytracingTier > D3D12_RAYTRACING_TIER_NOT_SUPPORTED) {
			file_ << "[Feature Support::D3D12_FEATURE_D3D12_OPTIONS5] raytracing tier: " << option.RaytracingTier << std::endl;

		} else {
			file_ << "[Feature Support::D3D12_FEATURE_D3D12_OPTIONS5] raytracing not support." << std::endl;
		}
	}

	{ //!< option7 (mesh shader)

		D3D12_FEATURE_DATA_D3D12_OPTIONS7 option = {};
		auto hr = device_->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS7, &option, sizeof(D3D12_FEATURE_DATA_D3D12_OPTIONS7));

		if (SUCCEEDED(hr) && option.MeshShaderTier > D3D12_MESH_SHADER_TIER_NOT_SUPPORTED) {
			file_ << "[Feature Support::D3D12_FEATURE_D3D12_OPTIONS7] mesh shader support." << std::endl;

		} else {
			file_ << "[Feature Support::D3D12_FEATURE_D3D12_OPTIONS7] mesh shader not support." << std::endl;
		}
	}

}
