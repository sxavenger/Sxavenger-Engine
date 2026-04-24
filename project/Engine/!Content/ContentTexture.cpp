#include "ContentTexture.h"
SXAVENGER_ENGINE_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* asset
#include "../Assets/Asset/AssetStorage.h"
#include "../Assets/Asset/AssetTexture.h"

//* engine
#include <Engine/System/System.h>

////////////////////////////////////////////////////////////////////////////////////////////
// Texture class methods
////////////////////////////////////////////////////////////////////////////////////////////

void Content::Texture::Attach(const std::filesystem::path& filepath, const std::any& parameter) {

	//!< 引数の保存
	filepath_  = filepath;
	parameter_ = parameter;

	// Uuidの割り当て
	AttachUuid(filepath);



}

void Content::Texture::AttachUuid(const std::filesystem::path& filepath) {
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
