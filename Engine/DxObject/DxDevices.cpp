#include "DxDevices.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include <Logger.h>

////////////////////////////////////////////////////////////////////////////////////////////
// Devices methods
////////////////////////////////////////////////////////////////////////////////////////////

DxObject::Devices::Devices() { Init(); }

DxObject::Devices::~Devices() { Term(); }

void DxObject::Devices::Init() {
#ifdef _DEBUG
	// デバックレイヤーの生成
	{
		if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController_)))) {
			// デバックレイヤーの有効化
			debugController_->EnableDebugLayer();
			// GPU側も有効化
			debugController_->SetEnableGPUBasedValidation(TRUE);

			Log("[dxObject.Devices]: debugController_ << Complete Create \n");
		}
	}
#endif // _DEBUG

	// DXGIファクトリーの生成
	{
		auto hr = CreateDXGIFactory(IID_PPV_ARGS(&dxgiFactory_));
		assert(SUCCEEDED(hr));

		Log("[dxObject.Devices]: dxgiFactory_ << Complete Create \n");
	}

	// アダプタの生成
	{
		// アダプタの読み込み
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
					Log(std::format(L" -- Use Adapter: {}\n", desc.Description));
					break;
				}

				// ソフトウェアアダプタの場合, nullptrを代入
				useAdapter_ = nullptr;
			}

			assert(useAdapter_ != nullptr);
			Log("[dxObject.Devices]: useAdapter_ << Complete Create \n");
		}
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
				Log(std::format(" -- FeatureLevel: {}\n", featureLevelStrings[i]));
				break;
			}
		}

		assert(device_ != nullptr);

		Log("[dxObject.Devices]: device_ << Complete Create \n");
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

			Log("// Complete Path >> infoQueue_ \n");
		}
	}
#endif // _DEBUG
}

