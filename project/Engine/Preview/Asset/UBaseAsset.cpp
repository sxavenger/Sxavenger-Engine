#include "UBaseAsset.h"

////////////////////////////////////////////////////////////////////////////////////////////
// UBaseAsset class methods
////////////////////////////////////////////////////////////////////////////////////////////

json UBaseAsset::Serialize() const {
	json data;
	data["uuid"] = uuid_.Serialize();
	data["name"] = name_;
	return data;
}

void UBaseAsset::Deserialize(const json& data) {
	uuid_ = Uuid::Deserialize(data.at("uuid").get<std::string>());
	name_ = data.at("name").get<std::string>();
}

void UBaseAsset::WaitComplete() const {
	while (!IsComplete()) {
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
		Logger::CommentRuntime("waiting for UAsset to complete loading...", "name: " + name_);
	}
}
