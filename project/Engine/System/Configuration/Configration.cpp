#include "Configration.h"
SXAVENGER_ENGINE_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/Utility/StreamLogger.h>

//* lib
#include <Lib/Adapter/Json/JsonHandler.h>
#include <Lib/Adapter/Json/JsonSerializer.h>

////////////////////////////////////////////////////////////////////////////////////////////
// Config structure methods
////////////////////////////////////////////////////////////////////////////////////////////

Configuration::Config::Config() {
	//* window
	title      = "Sxavenger Engine " + std::string(SXAVENGER_ENGINE_VERSION);
	resolution = { 1280, 720 };

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
	enableShaderOptimization = true;
}

void Configuration::Config::Load(const std::filesystem::path& filepath) {

	json data;
	if (!JsonHandler::LoadFromJson(filepath, data)) {
		StreamLogger::EngineLog("[Config] warning | user config load failed. request filepath: " + filepath.generic_string());
		return; //!< fileが見つからなければ初期設定の使用
	}

	StreamLogger::EngineLog("[Config] loaded user config. filepath: " + filepath.generic_string());

	//* window
	if (data.contains("title")) {
		title = JsonSerializeFormatter<std::string>::Deserialize(data["title"]);
	}

	if (data.contains("resolution")) {
		resolution = JsonSerializeFormatter<Vector2ui>::Deserialize(data["resolution"]);
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
	if (data.contains("enableShaderOptimization")) {
		enableShaderOptimization = JsonSerializeFormatter<bool>::Deserialize(data["enableShaderOptimization"]);
	}

}

void Configuration::Config::OutputLog() const {

	//* window
	StreamLogger::EngineLog(std::format("[Config] title: {}",      title));
	StreamLogger::EngineLog(std::format("[Config] resolution: {}", resolution));

	//* device
	StreamLogger::EngineLog(std::format("[Config] enableDebugLayer: {}",         enableDebugLayer));
	StreamLogger::EngineLog(std::format("[Config] enableGPUBasedValidation: {}", enableGPUBasedValidation));

	//* descriptor heap
	StreamLogger::EngineLog(std::format("[Config] descriptorCount_SRV_CBV_UAV: {}", descriptorCount_SRV_CBV_UAV));
	StreamLogger::EngineLog(std::format("[Config] descriptorCount_RTV: {}",         descriptorCount_RTV));
	StreamLogger::EngineLog(std::format("[Config] descriptorCount_DSV: {}",         descriptorCount_DSV));

	//* tearing
	StreamLogger::EngineLog(std::format("[Config] tearing: {}", isTearingAllowed));

	//* frame rate lock
	StreamLogger::EngineLog(std::format("[Config] frame rate lock: {}",   isLockFrameRate));
	StreamLogger::EngineLog(std::format("[Config] target frame rate: {}", targetFrameRate));

	//* shader optimization
	StreamLogger::EngineLog(std::format("[Config] shader optimization: {}", enableShaderOptimization));

}

////////////////////////////////////////////////////////////////////////////////////////////
// Support structure methods
////////////////////////////////////////////////////////////////////////////////////////////

void Configuration::Support::OutputLog() const {

	StreamLogger::EngineLog(std::format("[Support] isSupportTearing: {}",          isSupportTearing));
	StreamLogger::EngineLog(std::format("[Support] isSupportInlineRaytracing: {}", isSupportInlineRaytracing));

}

////////////////////////////////////////////////////////////////////////////////////////////
// Configuration class methods
////////////////////////////////////////////////////////////////////////////////////////////

void Configuration::Load() {
	config_.Load(kConfigFilepath_);
	config_.OutputLog();
}
