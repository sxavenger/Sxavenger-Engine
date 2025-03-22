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
	//* tiearing 
	isTearingAllowed = false;

	//* frame rate lock
	isLockFrameRate  = true;
	targetFrameRate  = 60.0f;
}

void SxavengerConfig::Config::Load(const std::filesystem::path& filepath) {
	json data;

	if (!JsonHandler::LoadFromJson(filepath, data)) {
		return; //!< fileが見つからなければ初期設定の使用
	}

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
};

//=========================================================================================
// static const variables
//=========================================================================================

const std::filesystem::path SxavengerConfig::kConfigFilepath_ = kPackagesDirectory / "config" / "config.json";

SxavengerConfig::Config SxavengerConfig::config_   = {};
SxavengerConfig::Support SxavengerConfig::support_ = {};

////////////////////////////////////////////////////////////////////////////////////////////
// SxavengeConfig class methods
////////////////////////////////////////////////////////////////////////////////////////////

void SxavengerConfig::Load() {
	config_.Load(kConfigFilepath_);
	OutputLog();
}

void SxavengerConfig::OutputLog() {
	EngineLog(std::format("[Config] tearing: {}",           config_.isTearingAllowed));
	EngineLog(std::format("[Config] frame rate lock: {}",   config_.isLockFrameRate));
	EngineLog(std::format("[Config] target frame rate: {}", config_.targetFrameRate));
}
