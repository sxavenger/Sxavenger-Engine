#include "UAssetStorage.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* lib
#include <Lib/Adapter/Json/JsonHandler.h>

////////////////////////////////////////////////////////////////////////////////////////////
// UAssetStorage class methods
////////////////////////////////////////////////////////////////////////////////////////////

UAssetStorage* UAssetStorage::GetInstance() {
	static UAssetStorage instance;
	return &instance;
}

void UAssetStorage::Serialize() const {
	json data = json::object();

	for (const auto& [id, items] : table_) {
		const std::string serialized = id.Serialize();

		data[serialized] = json::array();
		data[serialized].emplace_back(items.first.generic_string()); //!< filepath
		data[serialized].emplace_back(items.second);                 //!< type name
	}

	JsonHandler::WriteToJson(kTableFilepath_, data);
}

void UAssetStorage::Deserialize() {
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
}

const std::filesystem::path& UAssetStorage::GetFilepath(const Uuid& id) const {
	if (!table_.contains(id)) {
		throw std::runtime_error("Asset with the given ID does not exist.");
	}

	return table_.at(id).first;
}
