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
	//* device
	enableDebugLayer         = true;
	enableGPUBasedValidation = true;

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

	//* device
#ifdef _DEBUG
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
#ifdef _DEBUG
	Logger::EngineLog(std::format("[Config] enableDebugLayer: {}",          config_.enableDebugLayer));
	Logger::EngineLog(std::format("[Config] enableGPUBasedValidation: {}",  config_.enableGPUBasedValidation));
#endif

	Logger::EngineLog(std::format("[Config] tearing: {}",           config_.isTearingAllowed));

	Logger::EngineLog(std::format("[Config] frame rate lock: {}",   config_.isLockFrameRate));
	Logger::EngineLog(std::format("[Config] target frame rate: {}", config_.targetFrameRate));

	Logger::EngineLog(std::format("[Config] shader optimization: {}", config_.isEnableShaderOptimization));
}
