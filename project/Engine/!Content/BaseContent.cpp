#include "BaseContent.h"
SXAVENGER_ENGINE_USING

////////////////////////////////////////////////////////////////////////////////////////////
// BaseContent class methods
////////////////////////////////////////////////////////////////////////////////////////////

Content::BaseContent::BaseContent(Async::Execution execution) : execution_(execution) {
}

json Content::BaseContent::LoadMetaData(const std::filesystem::path& filepath) {
	std::filesystem::path meta = BaseContent::GetMetaFilepath(filepath);

	json data;
	JsonHandler::LoadFromJson(meta, data);

	return data;
}

void Content::BaseContent::SaveMetaData(const json& data, const std::filesystem::path& filepath) {
	std::filesystem::path meta = BaseContent::GetMetaFilepath(filepath);
	JsonHandler::WriteToJson(meta, data);
}

std::filesystem::path Content::BaseContent::GetMetaFilepath(const std::filesystem::path& filepath) {
	std::filesystem::path meta = filepath;
	meta += kMetaExtension;
	return meta;
}
