#include "UContentFont.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* uasset
#include "../Asset/UAssetStorage.h"

//* lib
#include <Lib/Adapter/Json/JsonHandler.h>

////////////////////////////////////////////////////////////////////////////////////////////
// UContentFont class methods
////////////////////////////////////////////////////////////////////////////////////////////

void UContentFont::AsyncLoad(_MAYBE_UNUSED const DirectXQueueContext* context) {
	UBaseContent::CheckExist();

	Load(context, UBaseContent::GetFilepath());
}

void UContentFont::AttachUuid() {
	UBaseContent::CheckExist();

	// idを取得
	GetUuid();

	// storageに登録
	auto asset = std::make_shared<UAssetFont>(id_);
	sUAssetStorage->Register(asset, UBaseContent::GetFilepath());
}

void UContentFont::ShowInspector() {
	UBaseContent::ShowInspector();

	if (ImGui::Button("Font")) {
		UBaseContent::SelectInspector(sUAssetStorage->GetAsset<UAssetFont>(id_).get());
	}
}

void UContentFont::Load(const DirectXQueueContext* context, const std::filesystem::path& filepath) {
	std::ifstream file(filepath, std::ios::binary);
	Exception::Assert(file.is_open(), "font load failed. filepath: " + filepath.generic_string());

	file.seekg(0, std::ios::end);
	size_t size = static_cast<size_t>(file.tellg());
	file.seekg(0, std::ios::beg);

	std::vector<uint8_t> buffer(size);
	file.read(reinterpret_cast<char*>(buffer.data()), size);
	file.close();

	stbtt_fontinfo info = {};
	if (!stbtt_InitFont(&info, buffer.data(), 0)) {
		Exception::Assert(false, "font info initialize failed.");
	}

	const float kFontSize = 64.0f; //!< todo: parameter化

	auto asset = sUAssetStorage->GetAsset<UAssetFont>(id_);
	asset->Setup(context, info, kFontSize);
}

void UContentFont::GetUuid() {
	std::filesystem::path filepath = UBaseContent::GetContentPath();

	if (JsonHandler::CheckExist(filepath)) {
		//!< Idが既に存在する場合は、Json形式で読み込む
		json data = JsonHandler::LoadFromJson(filepath);
		id_ = Uuid::Deserialize(data["id"].get<std::string>());

	} else {
		//!< 新しくIdを生成し, Json形式で保存する
		id_ = Uuid::Generate();

		json data = json::object();
		data["id"] = id_.Serialize();

		JsonHandler::WriteToJson(filepath, data);
	}
}
