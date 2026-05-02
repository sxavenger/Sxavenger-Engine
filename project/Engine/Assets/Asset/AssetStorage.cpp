#include "AssetStorage.h"
SXAVENGER_ENGINE_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/Utility/StreamLogger.h>

//* lib
#include <Lib/Adapter/Json/JsonHandler.h>

////////////////////////////////////////////////////////////////////////////////////////////
// Storage class methods
////////////////////////////////////////////////////////////////////////////////////////////

void AssetStorage::Term() {
	location_.clear();
	storage_.clear();
}

void AssetStorage::SerializeLocation() const {

	json data = json::object();

	for (const auto& [id, filepath] : location_) {
		data[id.Serialize()] = filepath.generic_string();
	}

	JsonHandler::WriteToJson(kLocationFilepath, data);
	StreamLogger::EngineLog("[Asset::Storage] asset storage location serialized.");
}

void AssetStorage::DeserializeLocation() {

	if (!JsonHandler::CheckExist(kLocationFilepath)) {
		return; //!< ファイルが存在しない場合は何もしない
	}

	json data = JsonHandler::LoadFromJson(kLocationFilepath);

	for (const auto& [id, filepath] : data.items()) {
		location_[Uuid::Deserialize(id)] = std::filesystem::path(filepath.get<std::string>());
	}

	StreamLogger::EngineLog("[Asset::Storage] asset storage location deserialized.");
}

const std::filesystem::path& AssetStorage::GetLocation(const Uuid& id) const {
	StreamLogger::AssertA(location_.contains(id), "asset storage location does not exist. uuid: " + id.Serialize());
	return location_.at(id);
}

AssetStorage* AssetStorage::GetInstance() {
	static AssetStorage instance;
	return &instance;
}
