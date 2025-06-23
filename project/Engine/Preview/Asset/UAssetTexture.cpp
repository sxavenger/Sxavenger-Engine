#include "UAssetTexture.h"

////////////////////////////////////////////////////////////////////////////////////////////
// UAssetTexture class methods
////////////////////////////////////////////////////////////////////////////////////////////

void UAssetTexture::Load(const DirectXThreadContext* context) {
	texture_.Load(context, filepath_);
}

json UAssetTexture::Serialize() const {
	json data = UBaseAsset::Serialize();
	data["filepath"] = filepath_.generic_string();

	return data;
}

void UAssetTexture::Deserialize(const json& data) {
	UBaseAsset::Deserialize(data);
	filepath_ = data.at("filepath").get<std::string>();

	// todo: Loadが必須
}
