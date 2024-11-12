#include "DxDevices.h"
_DXOBJECT_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include <windows.h>
#include <psapi.h>

////////////////////////////////////////////////////////////////////////////////////////////
// Devices methods
////////////////////////////////////////////////////////////////////////////////////////////

void Devices::Init() {

	if (CheckRunningPIX()) {
		useDebugLayer_ = false;
	}

#ifdef _DEBUG
	// デバックレイヤーの生成
	{
		if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController_)))) {
			if (useDebugLayer_) {
				// デバックレイヤーの有効化
				debugController_->EnableDebugLayer();
				// GPU側も有効化
				debugController_->SetEnableGPUBasedValidation(TRUE);
			}

			EngineLog(std::format("[_DXOBEJCT] Debug Layer : {}", useDebugLayer_));
		}
	}
#endif // _DEBUG

	// DXGIファクトリーの生成
	{
		auto hr = CreateDXGIFactory(IID_PPV_ARGS(&dxgiFactory_));
		Assert(SUCCEEDED(hr));
	}

	// アダプタの生成
	{
		for (UINT i = 0;
			dxgiFactory_->EnumAdapterByGpuPreference(i, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&useAdapter_)) != DXGI_ERROR_NOT_FOUND;
			++i) {

			// アダプタ情報を取得
			DXGI_ADAPTER_DESC3 desc = {};
			auto hr = useAdapter_->GetDesc3(&desc);
			Assert(SUCCEEDED(hr));

			// ソフトウェアアダプタじゃない場合, 成功
			if (!(desc.Flags & DXGI_ADAPTER_FLAG3_SOFTWARE)) {
				EngineLog(std::format(L"[_DXOBEJCT] Use Adapter: {}", desc.Description));
				break;
			}

			// ソフトウェアアダプタの場合, nullptrを代入
			useAdapter_ = nullptr;
		}

		Assert(useAdapter_ != nullptr);
	}

	// デバイスを生成
	{
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
				EngineLog(std::format("[_DXOBEJCT] D3D_FEATURE_LEVEL: {}", featureLevelStrings[i]));
				break;
			}
		}

		Assert(device_ != nullptr);
	}

#ifdef _DEBUG
	// プログラムを停止する機能
	{
		if (SUCCEEDED(device_->QueryInterface(IID_PPV_ARGS(&infoQueue_)))) {
			// プログラムを停止する種類
			infoQueue_->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, true);
			infoQueue_->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, true);
			//infoQueue_->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, true);

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
#endif // _DEBUG

	// サポートの確認
	isRayTracingEnabled_ = CheckRayTracingEnable();
	isMeshShaderEnabled_ = CheckMeshShaderEnable();

	// 仮でAssertを出しておく
	Assert(isRayTracingEnabled_, "raytracing version failed.");
	Assert(isMeshShaderEnabled_, "mesh shader version failed.");

	EngineLog("[_DXOBJECT]::Device complete init.");
}

void Devices::Term() {
}

bool Devices::CheckRunningPIX() {
	 // 確認したいDLL名
	const wchar_t* pixDllName = L"WinPixGpuCapturer.dll";

	// 現在のプロセスのハンドルを取得
	HANDLE processHandle = GetCurrentProcess();

	// プロセスに読み込まれているモジュールのリストを取得
	HMODULE modules[1024];
	DWORD cbNeeded;

	if (EnumProcessModules(processHandle, modules, sizeof(modules), &cbNeeded)) {
		int moduleCount = cbNeeded / sizeof(HMODULE);
		for (int i = 0; i < moduleCount; ++i) {
			wchar_t moduleName[MAX_PATH];
			// 各モジュールの名前を取得
			if (GetModuleBaseNameW(processHandle, modules[i], moduleName, sizeof(moduleName) / sizeof(wchar_t))) {
				if (wcscmp(moduleName, pixDllName) == 0) {
					return true; // PIXのDLLが見つかった場合
				}
			}
		}
	}

	return false; // PIXのDLLが見つからなかった場合
}

bool Devices::CheckRayTracingEnable() {

	// RayTracingのサポートのチェック
	{
		D3D12_FEATURE_DATA_D3D12_OPTIONS5 option = {};
		auto hr = device_->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS5, &option, sizeof(option));

		if (FAILED(hr) || option.RaytracingTier < D3D12_RAYTRACING_TIER_1_0) {
			EngineLog("Warning : RayTracing is not supported.");
			return false; //!< RayTracingがサポートされていない
		}
	}

	return true;
}

bool Devices::CheckMeshShaderEnable() {

	// シェーダーモデルをチェック
	{ //!< mesh shader の shader model が 6.5 以上である必要がある
		D3D12_FEATURE_DATA_SHADER_MODEL shaderModel = { D3D_SHADER_MODEL_6_5 };
		auto hr = device_->CheckFeatureSupport(D3D12_FEATURE_SHADER_MODEL, &shaderModel, sizeof(shaderModel));

		if (FAILED(hr) || (shaderModel.HighestShaderModel < D3D_SHADER_MODEL_6_5)) {
			EngineLog("Warning : Shader Model 6.5 is not supported.");
			return false; //!< shader modelが6.5以下しかサポートされてない
		}
	}

	// メッシュシェーダーのサポートをチェック
	{
		D3D12_FEATURE_DATA_D3D12_OPTIONS7 features = {};
		auto hr = device_->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS7, &features, sizeof(features));

		if (FAILED(hr) || (features.MeshShaderTier == D3D12_MESH_SHADER_TIER_NOT_SUPPORTED)) {
			EngineLog("Warning : Mesh Shaders aren't supported.");
			return false; //!< mesh shaderがサポートされてない
		}
	}

	return true;
}
