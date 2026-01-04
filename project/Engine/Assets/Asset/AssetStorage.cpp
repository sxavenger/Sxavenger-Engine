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
// AssetStorage class methods
////////////////////////////////////////////////////////////////////////////////////////////

AssetStorage* AssetStorage::GetInstance() {
	static AssetStorage instance;
	return &instance;
}

void AssetStorage::Serialize() const {
	json data = json::object();

	for (const auto& [id, items] : table_) {
		const std::string serialized = id.Serialize();

		data[serialized] = json::array();
		data[serialized].emplace_back(items.first.generic_string()); //!< filepath
		data[serialized].emplace_back(items.second);                 //!< type name
	}

	JsonHandler::WriteToJson(kTableFilepath_, data);
	StreamLogger::EngineLog("[AssetStorage]: asset storage uuid table serialized.");
}

void AssetStorage::Deserialize() {
	if (!JsonHandler::CheckExist(kTableFilepath_)) {
		return; //!< ファイルが存在しない場合は何もしない
	}

	json data = JsonHandler::LoadFromJson(kTableFilepath_);

	for (const auto& [id, items] : data.items()) {
		table_[Uuid::Deserialize(id)] = std::make_pair<std::filesystem::path, std::string>(
			items[0].get<std::string>(), //!< filepath
			items[1].get<std::string>()  //!< type name
		);
	}

	StreamLogger::EngineLog("[AssetStorage]: asset storage uuid table deserialized.");
}

const std::filesystem::path& AssetStorage::GetFilepath(const Uuid& id) const {
	StreamLogger::AssertA(table_.contains(id), "asset storage uuid table does not exist. uuid: " + id.Serialize());

	return table_.at(id).first;
}
