#include "DxDevices.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include <Logger.h>

////////////////////////////////////////////////////////////////////////////////////////////
// Devices methods
////////////////////////////////////////////////////////////////////////////////////////////

void DxObject::Devices::Init() {
#ifdef _DEBUG
	// デバックレイヤーの生成
	{
		if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController_)))) {
			// デバックレイヤーの有効化
			debugController_->EnableDebugLayer();
			// GPU側も有効化
			debugController_->SetEnableGPUBasedValidation(TRUE);

			Log("[DxObject::Devices]: debugController_ << Complete Create");
		}
	}
#endif // _DEBUG

	// DXGIファクトリーの生成
	{
		auto hr = CreateDXGIFactory(IID_PPV_ARGS(&dxgiFactory_));
		assert(SUCCEEDED(hr));

		Log("[DxObject::Devices]: dxgiFactory_ << Complete Create");
	}

	// アダプタの生成
	{
		for (UINT i = 0;
			dxgiFactory_->EnumAdapterByGpuPreference(i, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&useAdapter_)) != DXGI_ERROR_NOT_FOUND;
			++i) {

			// アダプタ情報を取得
			DXGI_ADAPTER_DESC3 desc = {};
			auto hr = useAdapter_->GetDesc3(&desc);
			assert(SUCCEEDED(hr));

			// ソフトウェアアダプタじゃない場合, 成功
			if (!(desc.Flags & DXGI_ADAPTER_FLAG3_SOFTWARE)) {
				Log(std::format(L" -- Use Adapter: {}", desc.Description));
				break;
			}

			// ソフトウェアアダプタの場合, nullptrを代入
			useAdapter_ = nullptr;
		}

		assert(useAdapter_ != nullptr);
		Log("[DxObject::Devices]: useAdapter_ << Complete Create");
	}

	// デバイスを生成
	{
		// 機能レベルとログ出力用の文字列
		D3D_FEATURE_LEVEL featureLevels[] = {
			D3D_FEATURE_LEVEL_12_2, D3D_FEATURE_LEVEL_12_1, D3D_FEATURE_LEVEL_12_0
		};

		const char* featureLevelStrings[] = { "12.2", "12.1", "12.0" };

		// 高い順に生成できるか確認
		for (size_t i = 0; i < _countof(featureLevels); ++i) {
			// 採用したアダプターでデバイスを生成
			auto hr = D3D12CreateDevice(useAdapter_.Get(), featureLevels[i], IID_PPV_ARGS(&device_));
			if (SUCCEEDED(hr)) {
				// 生成できたのでログ出力してループを抜ける
				Log(std::format(" -- FeatureLevel: {}", featureLevelStrings[i]));
				break;
			}
		}

		assert(device_ != nullptr);

		Log("[DxObject::Devices]: device_ << Complete Create");
	}

#ifdef _DEBUG
	// プログラムを停止する機能
	{
		ID3D12InfoQueue* infoQueue = nullptr;

		if (SUCCEEDED(device_->QueryInterface(IID_PPV_ARGS(&infoQueue)))) {
			// プログラムを停止する種類
			infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, true);
			infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, true);
			/*infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, true);*/

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
			infoQueue->PushStorageFilter(&filter);

			// 解放
			infoQueue->Release();

			Log("// Complete Path >> infoQueue_");
		}
	}
#endif // _DEBUG

	// サポートの確認
	isRayTracingEnabled_ = CheckRayTracingEnable();
	isMeshShaderEnabled_ = CheckMeshShaderEnable();

	// 仮でassertを出しておく
	assert(isRayTracingEnabled_);
	assert(isMeshShaderEnabled_);

}

void DxObject::Devices::Term() {
}

bool DxObject::Devices::CheckRayTracingEnable() {

	// RayTracingのサポートのチェック
	{
		D3D12_FEATURE_DATA_D3D12_OPTIONS5 option = {};
		auto hr = device_->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS5, &option, sizeof(option));

		if (FAILED(hr) || option.RaytracingTier < D3D12_RAYTRACING_TIER_1_0) {
			Log("Warning : RayTracing is not supported.");
			return false; //!< RayTracingがサポートされていない
		}
	}

	return true;
}

bool DxObject::Devices::CheckMeshShaderEnable() {

	// シェーダーモデルをチェック
	{ //!< mesh shader の shader model が 6.5 以上である必要がある
		D3D12_FEATURE_DATA_SHADER_MODEL shaderModel = { D3D_SHADER_MODEL_6_5 };
		auto hr = device_->CheckFeatureSupport(D3D12_FEATURE_SHADER_MODEL, &shaderModel, sizeof(shaderModel));

		if (FAILED(hr) || (shaderModel.HighestShaderModel < D3D_SHADER_MODEL_6_5)) {
			Log("Warning : Shader Model 6.5 is not supported.");
			return false; //!< shader modelが6.5以下しかサポートされてない
		}
	}

	// メッシュシェーダーのサポートをチェック
	{
		D3D12_FEATURE_DATA_D3D12_OPTIONS7 features = {};
		auto hr = device_->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS7, &features, sizeof(features));

		if (FAILED(hr) || (features.MeshShaderTier == D3D12_MESH_SHADER_TIER_NOT_SUPPORTED)) {
			Log("Warning : Mesh Shaders aren't supported.");
			return false; //!< mesh shaderがサポートされてない
		}
	}

	return true;
}
