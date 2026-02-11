#include "ContentFont.h"
SXAVENGER_ENGINE_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* asset
#include "../Asset/AssetStorage.h"

//* engine
#include <Engine/System/Utility/StreamLogger.h>

//* lib
#include <Lib/Adapter/Json/JsonHandler.h>

////////////////////////////////////////////////////////////////////////////////////////////
// ContentFont class methods
////////////////////////////////////////////////////////////////////////////////////////////

void ContentFont::AsyncLoad(MAYBE_UNUSED const DirectXQueueContext* context) {
	BaseContent::CheckExist();

	Load(context, BaseContent::GetFilepath());
}

void ContentFont::AttachUuid() {
	BaseContent::CheckExist();

	// idを取得
	AssignUuid();

	// storageに登録
	auto asset = std::make_shared<AssetFont>(id_);
	sAssetStorage->Register(asset, BaseContent::GetFilepath());
}

void ContentFont::ShowInspector() {
	BaseContent::ShowInspector();

	if (ImGui::Button("Font")) {
		BaseContent::SelectInspector(sAssetStorage->GetAsset<AssetFont>(id_).get());
	}
}

void ContentFont::Load(const DirectXQueueContext* context, const std::filesystem::path& filepath) {
	std::ifstream file(filepath, std::ios::binary);
	StreamLogger::AssertA(file.is_open(), "font load failed. filepath: " + filepath.generic_string());

	file.seekg(0, std::ios::end);
	size_t size = static_cast<size_t>(file.tellg());
	file.seekg(0, std::ios::beg);

	std::vector<uint8_t> buffer(size);
	file.read(reinterpret_cast<char*>(buffer.data()), size);
	file.close();

	stbtt_fontinfo info = {};
	if (!stbtt_InitFont(&info, buffer.data(), 0)) {
		StreamLogger::Exception("font info initialize failed.");
	}

	const float kFontSize = 64.0f; //!< todo: parameter化

	auto asset = sAssetStorage->GetAsset<AssetFont>(id_);
	asset->Setup(context, info, kFontSize);
}

void ContentFont::AssignUuid() {

	json meta = BaseContent::LoadMeta();

	if (meta.contains("id")) {
		//!< idが既に存在する場合は、metaから取得する
		id_ = Uuid::Deserialize(meta["id"].get<std::string>());

	} else {
		//!< idが存在しない場合は、新しくidを生成し, metaに保存する
		id_ = Uuid::Generate();

		meta["id"] = id_.Serialize();
		BaseContent::SaveMeta(meta);
	}
}
