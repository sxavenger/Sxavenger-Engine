#include "SxavengerConfig.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/Utility/Logger.h>

//* lib
#include <Lib/Adapter/Json/JsonHandler.h>
#include <Lib/Adapter/Json/JsonSerializer.h>

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

	//* descriptor heap
	descriptorCount_RTV         = 24;
	descriptorCount_DSV         = 8;
	descriptorCount_SRV_CBV_UAV = 512;

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
		Logger::EngineLog("[Config] warning | user config load failed. request filepath: " + filepath.generic_string());
		return; //!< fileが見つからなければ初期設定の使用
	}

	Logger::EngineLog("[Config] loaded user config. filepath: " + filepath.generic_string());

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
			Logger::EngineLog("[Config] warning | Invalid window size format.");
		}
	}

	if (data.contains("windowTitle")) {
		windowTitle = JsonSerializeFormatter<std::string>::Deserialize(data["windowTitle"]);
	}

	//* device
#ifdef _DEVELOPMENT
	if (data.contains("enableDebugLayer")) {
		enableDebugLayer = JsonSerializeFormatter<bool>::Deserialize(data["enableDebugLayer"]);
	}

	if (data.contains("enableGPUBasedValidation")) {
		enableGPUBasedValidation = JsonSerializeFormatter<bool>::Deserialize(data["enableGPUBasedValidation"]);
	}
#endif

	//* descriptor heap
	if (data.contains("descriptorCount")) {
		descriptorCount_RTV         = JsonSerializeFormatter<uint32_t>::Deserialize(data["descriptorCount"]["RTV"]);
		descriptorCount_DSV         = JsonSerializeFormatter<uint32_t>::Deserialize(data["descriptorCount"]["DSV"]);
		descriptorCount_SRV_CBV_UAV = JsonSerializeFormatter<uint32_t>::Deserialize(data["descriptorCount"]["SRV_CBV_UAV"]);
	}

	//* tearing
	if (data.contains("isTearingAllowed")) {
		isTearingAllowed = data.at("isTearingAllowed");
		isTearingAllowed = JsonSerializeFormatter<bool>::Deserialize(data["isTearingAllowed"]);
	}

	//* frame rate lock
	if (data.contains("isLockFrameRate")) {
		isLockFrameRate = JsonSerializeFormatter<bool>::Deserialize(data["isLockFrameRate"]);
	}

	if (data.contains("targetFrameRate")) {
		targetFrameRate = JsonSerializeFormatter<float>::Deserialize(data["targetFrameRate"]);
	}

	//* shader optimization
	if (data.contains("isEnableShaderOptimization")) {
		isEnableShaderOptimization = JsonSerializeFormatter<bool>::Deserialize(data["isEnableShaderOptimization"]);
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

	//* descriptor heap
	Logger::EngineLog(std::format("[Config] descriptorCount_SRV_CBV_UAV: {}", config_.descriptorCount_SRV_CBV_UAV));
	Logger::EngineLog(std::format("[Config] descriptorCount_RTV: {}",         config_.descriptorCount_RTV));
	Logger::EngineLog(std::format("[Config] descriptorCount_DSV: {}",         config_.descriptorCount_DSV));

	//* tearing
	Logger::EngineLog(std::format("[Config] tearing: {}", config_.isTearingAllowed));

	//* frame rate lock
	Logger::EngineLog(std::format("[Config] frame rate lock: {}",   config_.isLockFrameRate));
	Logger::EngineLog(std::format("[Config] target frame rate: {}", config_.targetFrameRate));

	//* shader optimization
	Logger::EngineLog(std::format("[Config] shader optimization: {}", config_.isEnableShaderOptimization));
}
