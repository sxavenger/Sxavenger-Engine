#include "BaseContent.h"
SXAVENGER_ENGINE_USING


//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/Utility/StreamLogger.h>
#include <Engine/System/Utility/RuntimeLogger.h>

//* c++
#include <thread>

////////////////////////////////////////////////////////////////////////////////////////////
// BaseContent class methods
////////////////////////////////////////////////////////////////////////////////////////////

void BaseContent::Attach(const std::filesystem::path& filepath, const std::any& parameter) {

	//!< 引数の保存
	filepath_  = filepath;
	parameter_ = parameter;

}

void BaseContent::WaitComplete() const {
	while (status_ != Status::Complete) {
		RuntimeLogger::LogDebug("[BaseContent]", "waiting for content to complete loading... filepath: " + filepath_.generic_string());
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}
}

json BaseContent::LoadMetaData(const std::filesystem::path& filepath) {
	std::filesystem::path meta = BaseContent::GetMetaFilepath(filepath);

	json data;
	JsonHandler::LoadFromJson(meta, data);

	return data;
}

void BaseContent::SaveMetaData(const json& data, const std::filesystem::path& filepath) {
	std::filesystem::path meta = BaseContent::GetMetaFilepath(filepath);
	JsonHandler::WriteToJson(meta, data);
}

std::filesystem::path BaseContent::GetMetaFilepath(const std::filesystem::path& filepath) {
	std::filesystem::path meta = filepath;
	meta += kMetaExtension;
	return meta;
}
