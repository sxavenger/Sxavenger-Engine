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

void ContentFont::Attach(const std::filesystem::path& filepath, const std::any& parameter) {

	//!< 引数の保存
	BaseContent::Attach(filepath, parameter);

	//!< Uuidの割り当て
	AttachUuid(filepath);

	//!< Storageに登録
	sAssetStorage->Register<AssetFont>(id_, filepath);

}

void ContentFont::Load(MAYBE_UNUSED const DirectXQueueContext* context) {

	std::ifstream file(BaseContent::GetFilepath(), std::ios::binary);
	StreamLogger::AssertA(file.is_open(), "font load failed. filepath: " + BaseContent::GetFilepath().generic_string());

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

	std::shared_ptr<AssetFont> asset = sAssetStorage->Get<AssetFont>(id_);
	asset->Setup(context, info, kFontSize);

	BaseContent::SetComplete(); //!< 読み込み完了
}

void ContentFont::AttachUuid(const std::filesystem::path& filepath) {

	json meta = BaseContent::LoadMetaData(filepath);

	if (meta.contains("id")) {
		//!< idが既に存在する場合は、metaから取得する
		id_ = Uuid::Deserialize(meta["id"].get<std::string>());

	} else {
		//!< idが存在しない場合は、新しくidを生成し, metaに保存する
		id_ = Uuid::Generate();

		meta["id"] = id_.Serialize();
		BaseContent::SaveMetaData(meta, filepath);
	}

}
