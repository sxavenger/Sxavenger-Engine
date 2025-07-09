#include "SxavengerConfig.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/Utility/Logger.h>

//* lib
#include <Lib/Adapter/Json/JsonHandler.h>

////////////////////////////////////////////////////////////////////////////////////////////
// Config structure methods
////////////////////////////////////////////////////////////////////////////////////////////

SxavengerConfig::Config::Config() {
	//* window
	windowSize  = { 1280, 720 };
	windowTitle = "Sxavenger Engine " + kSxavengerEngineVersion;

	//* device
	enableDebugLayer         = false;
	enableGPUBasedValidation = false;

	//* tiearing
	isTearingAllowed = false;

	//* frame rate lock
	isLockFrameRate  = true;
	targetFrameRate  = 60.0f;

	//* shader optimization
	isEnableShaderOptimization = true;
}

void SxavengerConfig::Config::Load(const std::filesystem::path& filepath) {
	json data;

	if (!JsonHandler::LoadFromJson(filepath, data)) {
		return; //!< fileが見つからなければ初期設定の使用
	}

	Logger::EngineLog("loaded user config. filepath: " + filepath.generic_string());

	//* window
	if (data.contains("windowSize")) {
		const auto& size = data.at("windowSize");

		if (size.is_array() && size.size() == 2) {
			windowSize.x = size[0].get<uint32_t>();
			windowSize.y = size[1].get<uint32_t>();

		} else if (size.is_object()) {
			if (size.contains("x") && size.contains("y")) {
				windowSize.x = size.at("x").get<uint32_t>();
				windowSize.y = size.at("y").get<uint32_t>();

			} else if (size.contains("width") && size.contains("height")) {
				windowSize.x = size.at("width").get<uint32_t>();
				windowSize.y = size.at("height").get<uint32_t>();
			}

		} else {
			Logger::EngineLog("[Config] warning | Invalid windowSize format.");
		}
	}

	if (data.contains("windowTitle")) {
		windowTitle = data.at("windowTitle");
	}

	//* device
#ifdef _DEVELOPMENT
	if (data.contains("enableDebugLayer")) {
		enableDebugLayer = data.at("enableDebugLayer");
	}

	if (data.contains("enableGPUBasedValidation")) {
		enableGPUBasedValidation = data.at("enableGPUBasedValidation");
	}
#endif

	//* tearing
	if (data.contains("isTearingAllowed")) {
		isTearingAllowed = data.at("isTearingAllowed");
	}

	//* frame rate lock
	if (data.contains("isLockFrameRate")) {
		isLockFrameRate = data.at("isLockFrameRate");
	}

	if (data.contains("targetFrameRate")) {
		targetFrameRate = data.at("targetFrameRate");
	}

	//* shader optimization
	if (data.contains("isEnableShaderOptimization")) {
		isEnableShaderOptimization = data.at("isEnableShaderOptimization");
	}
};

////////////////////////////////////////////////////////////////////////////////////////////
// SxavengeConfig class methods
////////////////////////////////////////////////////////////////////////////////////////////

void SxavengerConfig::Load() {
	config_.Load(kConfigFilepath_);
	OutputLog();
}

void SxavengerConfig::OutputLog() {

	//* window
	Logger::EngineLog(std::format("[Config] windowSize width: {} height: {}", config_.windowSize.x, config_.windowSize.y));
	Logger::EngineLog(std::format("[Config] windowTitle: {}",                 config_.windowTitle));

#ifdef _DEVELOPMENT
	//* device
	Logger::EngineLog(std::format("[Config] enableDebugLayer: {}",          config_.enableDebugLayer));
	Logger::EngineLog(std::format("[Config] enableGPUBasedValidation: {}",  config_.enableGPUBasedValidation));
#endif

	//* tearing
	Logger::EngineLog(std::format("[Config] tearing: {}", config_.isTearingAllowed));

	//* frame rate lock
	Logger::EngineLog(std::format("[Config] frame rate lock: {}",   config_.isLockFrameRate));
	Logger::EngineLog(std::format("[Config] target frame rate: {}", config_.targetFrameRate));

	//* shader optimization
	Logger::EngineLog(std::format("[Config] shader optimization: {}", config_.isEnableShaderOptimization));
}
