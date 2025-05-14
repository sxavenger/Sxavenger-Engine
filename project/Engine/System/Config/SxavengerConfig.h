#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
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

		//* device
		bool enableDebugLayer;
		bool enableGPUBasedValidation;

		//* tiearing
		bool isTearingAllowed;

		//* frame rate lock
		bool isLockFrameRate;
		float targetFrameRate;
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
	static const std::filesystem::path kConfigFilepath_;

	//* config
	static Config config_;

	//* support
	static Support support_;

};
