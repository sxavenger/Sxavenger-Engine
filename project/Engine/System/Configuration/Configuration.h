#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/Foundation.h>

//* lib
#include <Lib/Math/Vector2.h>

//* c++
#include <string>
#include <filesystem>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////////////
// Directory
////////////////////////////////////////////////////////////////////////////////////////////

static const std::filesystem::path kPackagesDirectory = "packages";

static const std::filesystem::path kAssetsDirectory = "assets";

////////////////////////////////////////////////////////////////////////////////////////////
// Configuration class
////////////////////////////////////////////////////////////////////////////////////////////
//! @brief エンジンの起動設定(解像度/デバッグ層/フレームレート等)をjsonから読み込み保持する静的クラス
class Configuration {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// Config structure
	////////////////////////////////////////////////////////////////////////////////////////////
	//! @brief 各種起動設定値を保持する構造体
	struct Config {
	public:

		//=========================================================================================
		// public methods
		//=========================================================================================

		Config();

		//! @brief 指定jsonファイルから設定を読み込む
		//! @param[in] filepath 設定ファイルのパス
		void Load(const std::filesystem::path& filepath);

		//! @brief 現在の設定値をログ出力する
		void OutputLog() const;

		//=========================================================================================
		// public variables
		//=========================================================================================

		//* window
		std::string title;
		Vector2ui resolution;

		//* device
		bool enableDebugLayer;
		bool enableGPUBasedValidation;

		//* descriptor heap
		uint32_t descriptorCount_SRV_CBV_UAV;
		uint32_t descriptorCount_RTV;
		uint32_t descriptorCount_DSV;

		//* tearing
		bool isTearingAllowed;

		//* frame rate lock
		bool isLockFrameRate;
		float targetFrameRate;

		//* shader optimization
		bool enableShaderOptimization;

		//* pix
		bool enablePix;

	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// Support structure
	////////////////////////////////////////////////////////////////////////////////////////////
	//! @brief GPUの機能サポート状況を保持する構造体 (TODO: Device側へ移行予定)
	struct Support {
		//!< TODO: 廃止予定. Device側を参照して機能サポートの有無を確認するようにする
	public:

		//=========================================================================================
		// public methods
		//=========================================================================================

		void OutputLog() const;

		//=========================================================================================
		// public variables
		//=========================================================================================

		bool isSupportTearing;

	};

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//! @brief Configを読み込む
	//! @note [構成プロパティ]から設定を読み込む
	static void Load();

	//* getter *//

	static const Config& GetConfig() { return config_; }
	static Config& FixConfig() { return config_; }

	static Support& GetSupport() { return support_; }

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* filepath
	static inline const std::string           kConfigFilename_ = std::string("config_") + std::string(_PROFILE) + std::string(".json");
	static inline const std::filesystem::path kConfigFilepath_ = kPackagesDirectory / "config" / kConfigFilename_;

	//* config
	static inline Config config_ = {};

	//* support
	static inline Support support_ = {};

};

SXAVENGER_ENGINE_NAMESPACE_END
