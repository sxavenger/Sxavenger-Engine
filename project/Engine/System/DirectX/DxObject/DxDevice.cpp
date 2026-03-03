#include "DxDevice.h"
SXAVENGER_ENGINE_USING
DXOBJECT_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/Utility/StreamLogger.h>
#include <Engine/System/Configuration/Configuration.h>

//* external
#include <magic_enum.hpp>

//* c++
#include <psapi.h>
#include <format>

////////////////////////////////////////////////////////////////////////////////////////////
// Device class methods
////////////////////////////////////////////////////////////////////////////////////////////

void Device::Init() {

	CreateDebugLayer();
	CreateFactory();
	CreateAdapter();
	CreateDevice();
	CreateInfoQueue();

	//!< 機能レベルの取得
	shaderModelTier_     = CheckShaderModelTier();
	raytracingTier_      = CheckRaytracingTier();
	isMeshShaderSupport_ = CheckMeshShaderEnable();

	//!< エンジン機能要件との確認
	StreamLogger::AssertA(shaderModelTier_ >= DxObject::kRequireShaderModel, "require shader model is not supported.");
	StreamLogger::AssertA(raytracingTier_ >= D3D12_RAYTRACING_TIER_1_0, "raytracing is not supported.");
	StreamLogger::AssertA(isMeshShaderSupport_, "mesh shader is not supported.");

	StreamLogger::EngineLog("[DXOBJECT Device] complete initialize.");
}

void Device::Term() {
	StreamLogger::EngineLog("[DXOBJECT Device] term.");
}

void Device::CheckDeviceStatus() const {
	auto hr = device_->GetDeviceRemovedReason();

	if (SUCCEEDED(hr)) {
		return; //!< 正常
	}

	DxObject::Assert(hr, L"device removed.");
}

void Device::CreateDebugLayer() {
#ifdef _DEVELOPMENT

	// デバックレイヤーの生成
	if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController_)))) {
		if (Configuration::GetConfig().enableDebugLayer) {
			// デバックレイヤーの有効化
			debugController_->EnableDebugLayer();
		}

		if (Configuration::GetConfig().enableGPUBasedValidation) {
			// GPU側も有効化
			debugController_->SetEnableGPUBasedValidation(true);
		}
	}

#endif // _DEVELOPMENT
}

void Device::CreateFactory() {
	// DXGIファクトリーの生成
	auto hr = CreateDXGIFactory(IID_PPV_ARGS(&dxgiFactory_));
	DxObject::Assert(hr, L"factory create failed.");

	// ティアリングを確認
	if (Configuration::GetConfig().isTearingAllowed) {
		BOOL isTearingSupport = false;

		hr = dxgiFactory_->CheckFeatureSupport(DXGI_FEATURE_PRESENT_ALLOW_TEARING, &isTearingSupport, sizeof(BOOL));
		DxObject::Assert(hr, L"check feature support error.");

		if (!isTearingSupport) {
			StreamLogger::EngineLog("[DXOBEJCT Device] warning | tearing is not supported.");
		}

		// 設定を反映
		Configuration::GetSupport().isSupportTearing = isTearingSupport;
	}
}

void Device::CreateAdapter() {
	// アダプタの生成
	for (UINT i = 0;
		dxgiFactory_->EnumAdapterByGpuPreference(i, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&useAdapter_)) != DXGI_ERROR_NOT_FOUND;
		++i) {

		// アダプタ情報を取得
		DXGI_ADAPTER_DESC3 desc = {};
		auto hr = useAdapter_->GetDesc3(&desc);
		DxObject::Assert(hr, L"adapter failed.");

		// ソフトウェアアダプタじゃない場合, 成功
		if (!(desc.Flags & DXGI_ADAPTER_FLAG3_SOFTWARE)) {
			StreamLogger::EngineLog(std::format(L"[DXOBEJCT Device] Use Adapter: {}", desc.Description));
			break;
		}

		// ソフトウェアアダプタの場合, nullptrを代入
		useAdapter_ = nullptr;
	}

	StreamLogger::AssertA(useAdapter_ != nullptr, "adapter not found.");
}

void Device::CreateDevice() {
	// デバイスを生成
	// 機能レベルとログ出力用の文字列
	D3D_FEATURE_LEVEL featureLevels[] = {
		D3D_FEATURE_LEVEL_12_2, D3D_FEATURE_LEVEL_12_1, D3D_FEATURE_LEVEL_12_0
	};

	const LPCSTR featureLevelStrings[] = { "12.2", "12.1", "12.0" };

	// 高い順に生成できるか確認
	for (size_t i = 0; i < _countof(featureLevels); ++i) {
		// 採用したアダプターでデバイスを生成
		auto hr = D3D12CreateDevice(useAdapter_.Get(), featureLevels[i], IID_PPV_ARGS(&device_));
		if (SUCCEEDED(hr)) {
			// 生成できたのでログ出力してループを抜ける
			StreamLogger::EngineLog(std::format("[DXOBEJCT Device] D3D_FEATURE_LEVEL: {}", featureLevelStrings[i]));
			break;
		}
	}

	StreamLogger::AssertA(device_ != nullptr, "device create failed.");
}

void Device::CreateInfoQueue() {
#ifdef _DEVELOPMENT
	// プログラムを停止する機能
	{
		if (SUCCEEDED(device_->QueryInterface(IID_PPV_ARGS(&infoQueue_)))) {
			// プログラムを停止する種類
			infoQueue_->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, true);
			infoQueue_->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR,      true);
			//infoQueue_->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING,  true);

			// 抑制するメッセージのID
			D3D12_MESSAGE_ID denyIds[] = {
				D3D12_MESSAGE_ID_RESOURCE_BARRIER_MISMATCHING_COMMAND_LIST_TYPE
			};

			// 抑制するレベル
			D3D12_MESSAGE_SEVERITY serverities[] = { D3D12_MESSAGE_SEVERITY_INFO };

			D3D12_INFO_QUEUE_FILTER filter = {};
			filter.DenyList.NumIDs        = _countof(denyIds);
			filter.DenyList.pIDList       = denyIds;
			filter.DenyList.NumSeverities = _countof(serverities);
			filter.DenyList.pSeverityList = serverities;

			// 指定したメッセージの抑制
			infoQueue_->PushStorageFilter(&filter);
		}
	}
#endif // _DEVELOPMENT
}

D3D_SHADER_MODEL Device::CheckShaderModelTier() const {
	//!< Shader Modelのサポートのチェック

	D3D12_FEATURE_DATA_SHADER_MODEL model = { D3D_HIGHEST_SHADER_MODEL }; //!< 最大値での確認
	auto hr = device_->CheckFeatureSupport(D3D12_FEATURE_SHADER_MODEL, &model, sizeof(model));

	if (FAILED(hr)) {
		StreamLogger::EngineLog("[DXOBEJCT Device] warning | shader model is not supported.");
		return D3D_SHADER_MODEL_NONE; //!< Shader Model 5.1がサポートされていない
	}

	StreamLogger::EngineLog(std::format("[DXOBEJCT Device] feature levels - shader model: {}", magic_enum::enum_name(model.HighestShaderModel)));
	return model.HighestShaderModel;
}

D3D12_RAYTRACING_TIER Device::CheckRaytracingTier() const {
	//!< Raytracingのサポートのチェック
	
	D3D12_FEATURE_DATA_D3D12_OPTIONS5 option = {};
	auto hr = device_->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS5, &option, sizeof(option));

	StreamLogger::EngineLog(std::format("[DXOBEJCT Device] feature levels - raytracing tier: {}", magic_enum::enum_name(option.RaytracingTier)));

	if (FAILED(hr)) {
		StreamLogger::EngineLog("[DXOBEJCT Device] warning | raytracing is not supported.");
		return D3D12_RAYTRACING_TIER_NOT_SUPPORTED; //!< Raytracingがサポートされていない
	}

	if (option.RaytracingTier < D3D12_RAYTRACING_TIER_1_0) {
		StreamLogger::EngineLog("[DXOBEJCT Device] warning | raytracing is not supported.");
	}

	if (option.RaytracingTier < D3D12_RAYTRACING_TIER_1_1) {
		StreamLogger::EngineLog("[DXOBEJCT Device] warning | inline raytracing is not supported.");
	}

	return option.RaytracingTier;
	
}

bool Device::CheckMeshShaderEnable() const {
	//!< Mesh Shaderのサポートのチェック

	D3D12_FEATURE_DATA_D3D12_OPTIONS7 features = {};
	auto hr = device_->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS7, &features, sizeof(features));

	StreamLogger::EngineLog(std::format("[DXOBEJCT Device] feature levels - mesh shader tier: {}", magic_enum::enum_name(features.MeshShaderTier)));

	if (FAILED(hr) || (features.MeshShaderTier == D3D12_MESH_SHADER_TIER_NOT_SUPPORTED)) {
		StreamLogger::EngineLog("[DXOBEJCT Device] warning | mesh shaders aren't supported.");
		return false; //!< mesh shaderがサポートされてない
	}

	return true;
}
