#include "FeatureLevelExecution.h"
SXAVENGER_ENGINE_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/Utility/Convert.h>
#include <Engine/System/Utility/StreamLogger.h>

//* external
#include <magic_enum.hpp>

////////////////////////////////////////////////////////////////////////////////////////////
// FeatureLevelExecution class methods
////////////////////////////////////////////////////////////////////////////////////////////

void FeatureLevelExecution::Init(Execution::Context* context) {
	context->SetProcess(Execution::Process::Init, std::nullopt, [this]() { CheckFeatureLevel(); });
}

void FeatureLevelExecution::Term() {
}

void FeatureLevelExecution::CheckFeatureLevel() {

	StreamLogger::Init();
	StreamLogger::Log("[FeatureLevelExecution] >> check feature levels.");

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

	StreamLogger::Log("[FeatureLevelExecution] >> feature levels complete.");

}

bool FeatureLevelExecution::CheckFactory() {

	auto hr = CreateDXGIFactory(IID_PPV_ARGS(&factory_));
	StreamLogger::Log(std::format("[FeatureLevelExecution] >> [IDXGIFactory7] CreateDXGIFactory(...): {}", SUCCEEDED(hr)));

	if (FAILED(hr)) {
		return false;
	}

	BOOL isTearingSupport = false;
	hr = factory_->CheckFeatureSupport(DXGI_FEATURE_PRESENT_ALLOW_TEARING, &isTearingSupport, sizeof(BOOL));
	StreamLogger::Log(std::format("[FeatureLevelExecution] >> [IDXGIFactory7] CheckFeatureSupport(DXGI_FEATURE_PRESENT_ALLOW_TEARING) = {}", isTearingSupport));

	return true;
}

bool FeatureLevelExecution::CheckAdapter() {

	for (UINT i = 0;
		factory_->EnumAdapterByGpuPreference(i, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&adapter_)) != DXGI_ERROR_NOT_FOUND;
		++i) {

		// アダプタ情報を取得
		DXGI_ADAPTER_DESC3 desc = {};
		auto hr = adapter_->GetDesc3(&desc);

		if (FAILED(hr)) {
			StreamLogger::Log("[FeatureLevelExecution] >> [IDXGIAdapter4] GetDesc failed.");
			continue;
		}

		if (!(desc.Flags & DXGI_ADAPTER_FLAG3_SOFTWARE)) {
			StreamLogger::Log(std::format("[FeatureLevelExecution] >> [IDXGIAdapter4] Adapter Name: {}", ToString(desc.Description)));
			return true;
		}
	}

	StreamLogger::Log("[FeatureLevelExecution] >> [IDXGIAdapter4] Not found Hardware Adapter.");
	return false;
}

bool FeatureLevelExecution::CheckDevice() {

	static const std::pair<D3D_FEATURE_LEVEL, LPCSTR> featureLevels[] = {
		{ D3D_FEATURE_LEVEL_12_2, "12_2" },
		{ D3D_FEATURE_LEVEL_12_1, "12_1" },
		{ D3D_FEATURE_LEVEL_12_0, "12_0" },
	};

	for (size_t i = 0; i < _countof(featureLevels); ++i) {
		auto hr = D3D12CreateDevice(adapter_.Get(), featureLevels[i].first, IID_PPV_ARGS(&device_));
		if (SUCCEEDED(hr)) {
			StreamLogger::Log(std::format("[FeatureLevelExecution] >> [ID3D12Device8] feature level: {}", featureLevels[i].second));
			return true;
		}
	}

	StreamLogger::Log("[FeatureLevelExecution] >> [ID3D12Device8] feature level D3D_FEATURE_LEVEL_12_X not support.");
	return false;
}

void FeatureLevelExecution::CheckSupport() {

	
	{ //!< shader model

		D3D12_FEATURE_DATA_SHADER_MODEL shaderModel = { D3D_HIGHEST_SHADER_MODEL };

		auto hr = device_->CheckFeatureSupport(D3D12_FEATURE_SHADER_MODEL, &shaderModel, sizeof(D3D12_FEATURE_DATA_SHADER_MODEL));
		if (SUCCEEDED(hr)) {
			StreamLogger::Log(std::format("[FeatureLevelExecution] >> [Feature Support::D3D12_FEATURE_SHADER_MODEL]: {}", magic_enum::enum_name(shaderModel.HighestShaderModel)));

		} else {
			StreamLogger::Log("[FeatureLevelExecution] >> [Feature Support::D3D12_FEATURE_SHADER_MODEL]: Unknown.");
		}
	}

	{ //!< option5 (raytracing)

		D3D12_FEATURE_DATA_D3D12_OPTIONS5 option = {};
		auto hr = device_->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS5, &option, sizeof(D3D12_FEATURE_DATA_D3D12_OPTIONS5));

		if (SUCCEEDED(hr) && option.RaytracingTier > D3D12_RAYTRACING_TIER_NOT_SUPPORTED) {
			StreamLogger::Log(std::format("[FeatureLevelExecution] >> [Feature Support::D3D12_FEATURE_D3D12_OPTIONS5] raytracing tier: {}", magic_enum::enum_name(option.RaytracingTier)));

		} else {
			StreamLogger::Log("[FeatureLevelExecution] >> [Feature Support::D3D12_FEATURE_D3D12_OPTIONS5] raytracing not support.");
		}
	}

	{ //!< option7 (mesh shader)

		D3D12_FEATURE_DATA_D3D12_OPTIONS7 option = {};
		auto hr = device_->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS7, &option, sizeof(D3D12_FEATURE_DATA_D3D12_OPTIONS7));

		if (SUCCEEDED(hr) && option.MeshShaderTier > D3D12_MESH_SHADER_TIER_NOT_SUPPORTED) {
			StreamLogger::Log("[FeatureLevelExecution] >> [Feature Support::D3D12_FEATURE_D3D12_OPTIONS7] mesh shader support.");

		} else {
			StreamLogger::Log("[FeatureLevelExecution] >> [Feature Support::D3D12_FEATURE_D3D12_OPTIONS7] mesh shader not support.");
		}
	}

}
