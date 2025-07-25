#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* lib
#include <Lib/Geometry/Vector2.h>

//* c++
#include <string>
#include <filesystem>

////////////////////////////////////////////////////////////////////////////////////////////
// Version
////////////////////////////////////////////////////////////////////////////////////////////

static const std::string kSxavengerEngineVersion = "3.4";

////////////////////////////////////////////////////////////////////////////////////////////
// Directory
////////////////////////////////////////////////////////////////////////////////////////////

static const std::filesystem::path kPackagesDirectory       = "packages";
static const std::filesystem::path kPackagesShaderDirectory = kPackagesDirectory / "shaders";

static const std::filesystem::path kAssetsDirectory         = "assets";
static const std::filesystem::path kAssetsJsonDirectory     = kAssetsDirectory / "json";
static const std::filesystem::path kAssetsModelDirectory    = kAssetsDirectory / "models";

////////////////////////////////////////////////////////////////////////////////////////////
// SxavengeConfig class
////////////////////////////////////////////////////////////////////////////////////////////
class SxavengerConfig {
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

		//=========================================================================================
		// public variables
		//=========================================================================================

		//* window
		Vector2ui windowSize;
		std::string windowTitle;

		//* device
		bool enableDebugLayer;
		bool enableGPUBasedValidation;

		//* tiearing
		bool isTearingAllowed;

		//* frame rate lock
		bool isLockFrameRate;
		float targetFrameRate;

		//* shader optimization
		bool isEnableShaderOptimization;
	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// Support structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct Support {
	public:

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

	static void Load();

	static void OutputLog();

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
