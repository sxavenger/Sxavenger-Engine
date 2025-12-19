#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/Foundation.h>

//* lib
#include <Lib/Geometry/Vector2.h>

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
class Configuration {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// Config structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct Config {
	public:

		//=========================================================================================
		// public methods
		//=========================================================================================

		Config();

		void Load(const std::filesystem::path& filepath);

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

		//* tiearing
		bool isTearingAllowed;

		//* frame rate lock
		bool isLockFrameRate;
		float targetFrameRate;

		//* shader optimization
		bool enableShaderOptimization;

	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// Support structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct Support {
	public:

		//=========================================================================================
		// public methods
		//=========================================================================================

		void OutputLog() const;

		//=========================================================================================
		// public variables
		//=========================================================================================

		bool isSupportTearing;
		bool isSupportInlineRaytracing;

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
