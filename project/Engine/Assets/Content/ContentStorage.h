#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* content
#include "BaseContent.h"
#include "ContentTexture.h"
#include "ContentModel.h"
#include "ContentAnimation.h"
#include "ContentBlob.h"
#include "ContentScene.h"
#include "ContentFont.h"
#include "ContentAudio.h"
#include "ContentBehaviour.h"

//* engine
#include <Engine/Foundation.h>
#include <Engine/System/System.h>

//* lib
#include <Lib/Sxl/OptimizedPathMap.h>
#include <Lib/Adapter/Uuid/Uuid.h>

//* c++
#include <unordered_map>
#include <memory>
#include <typeinfo>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////////////
// ContentStorage class
////////////////////////////////////////////////////////////////////////////////////////////
//! @brief 型とファイルパスをキーにContentをインポート/キャッシュ管理するストレージ (シングルトン)
class ContentStorage {
public:
	// TODO: 別threadでContent自体の変更を検知する.

	////////////////////////////////////////////////////////////////////////////////////////////
	// using
	////////////////////////////////////////////////////////////////////////////////////////////

	using Storage = std::unordered_map<const std::type_info*, Sxl::OptimizedPathMap<std::shared_ptr<BaseContent>>>;

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	void Term() { storage_.clear(); }

	//* storage option *//

	//! @brief Contentの読み込み 
	//! @tparam T Contentの型
	//! @param[in] filepath Contentのファイルパス
	//! @param[in] parameter 読み込みパラメータ
	//! @return 読み込んだContentの共有ポインタ
	template <Content T>
	std::shared_ptr<T> Import(const std::filesystem::path& filepath, const std::any& parameter = std::any());

	//! @brief Contentの再読み込み
	//! @tparam T Contentの型
	//! @param[in] filepath Contentのファイルパス
	//! @param[in] parameter 読み込みパラメータ
	//! @return 読み込んだContentの共有ポインタ
	template <Content T>
	std::shared_ptr<T> Reload(const std::filesystem::path& filepath, const std::any& parameter = std::any());

	//! @brief Contentの直接登録
	//! @param[in] type Contentの型情報
	//! @param[in] content 登録するContentの共有ポインタ
	//! @param[in] filepath Contentのファイルパス
	//! @param[in] parameter 登録するContentのパラメータ
	void Emplace(const std::type_info* type, const std::shared_ptr<BaseContent>& content, const std::filesystem::path& filepath, const std::any& parameter);

	//! @brief Contentの登録（存在しない場合のみ）
	//! @param[in] type Contentの型情報
	//! @param[in] content 登録するContentの共有ポインタ
	//! @param[in] filepath Contentのファイルパス
	//! @param[in] parameter 登録するContentのパラメータ
	void TryEmplace(const std::type_info* type, const std::shared_ptr<BaseContent>& content, const std::filesystem::path& filepath, const std::any& parameter = std::any());

	//! @brief Contentの取得
	//! @tparam T Contentの型
	//! @param[in] filepath Contentのファイルパス
	//! @retval ptr     取得したContentの共有ポインタ
	//! @retval nullptr Contentが存在しない場合
	template <Content T>
	std::shared_ptr<T> GetContent(const std::filesystem::path& filepath) const;

	//! @brief Contentの取得
	//! @param[in] type Contentの型情報
	//! @param[in] filepath Contentのファイルパス
	//! @retval ptr 取得したContentの基底共有ポインタ
	//! @retval nullptr Contentが存在しない場合
	std::shared_ptr<BaseContent> GetContent(const std::type_info* type, const std::filesystem::path& filepath) const;

	//! @brief 指定したfilepathのContent型情報を取得
	//! @param[in] filepath Contentのファイルパス
	//! @retval ptr 取得したContentの型情報
	//! @retval nullptr Contentが存在しない場合
	const std::type_info* GetType(const std::filesystem::path& filepath) const;

	//* imgui option *//

	static void DragAndDropSource(const std::type_info* type, const std::filesystem::path& filepath);

	static std::optional<std::filesystem::path> DragAndDropTargetFilepath(const std::type_info* type);

	template <Content T>
	std::shared_ptr<T> DragAndDropTargetContent();

	template <Content T>
	void DragAndDropTargetContentFunc(const std::function<void(const std::shared_ptr<T>&)>& function);
	
	//* singleton *//

	static ContentStorage* GetInstance();

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	Storage storage_;

	//=========================================================================================
	// private methods
	//=========================================================================================

	//* helper methods *//

	static void CheckExists(const std::filesystem::path& filepath);

	template <Content T>
	static std::shared_ptr<T> Cast(const std::shared_ptr<BaseContent>& content);

	bool Contains(const std::type_info* type, const std::filesystem::path& filepath) const;

	

};

////////////////////////////////////////////////////////////////////////////////////////////
// ContentStorage class template methods
////////////////////////////////////////////////////////////////////////////////////////////

template <Content T>
inline std::shared_ptr<T> ContentStorage::Import(const std::filesystem::path& filepath, const std::any& parameter) {
	constexpr const std::type_info* type = &typeid(T);

	if (!ContentStorage::Contains(type, filepath)) {
		return ContentStorage::Reload<T>(filepath, parameter); //!< Contentが存在しない場合は、読み込みを行う
	}

	return ContentStorage::Cast<T>(storage_[type][filepath]);
}

template <Content T>
inline std::shared_ptr<T> ContentStorage::Reload(const std::filesystem::path& filepath, const std::any& parameter) {
	constexpr const std::type_info* type = &typeid(T);

	std::shared_ptr<BaseContent> content = std::make_shared<T>();
	ContentStorage::Emplace(type, content, filepath, parameter);

	return ContentStorage::Cast<T>(content);
}

template <Content T>
inline std::shared_ptr<T> ContentStorage::GetContent(const std::filesystem::path& filepath) const {
	constexpr const std::type_info* type = &typeid(T);

	if (!ContentStorage::Contains(type, filepath)) {
		return nullptr; //!< Contentが存在しない
	}

	return ContentStorage::Cast<T>(storage_.at(type).At(filepath)); //!< Contentを取得
}

template <Content T>
inline std::shared_ptr<T> ContentStorage::DragAndDropTargetContent() {
	constexpr const std::type_info* type = &typeid(T);

	std::optional<std::filesystem::path> filepath = ContentStorage::DragAndDropTargetFilepath(type);

	if (!filepath.has_value()) {
		return nullptr;
	}

	return ContentStorage::Import<T>(filepath.value());
}

template <Content T>
inline std::shared_ptr<T> ContentStorage::Cast(const std::shared_ptr<BaseContent>& content) {
	return std::static_pointer_cast<T>(content);
}

template <Content T>
inline void ContentStorage::DragAndDropTargetContentFunc(const std::function<void(const std::shared_ptr<T>&)>& function) {
	std::shared_ptr<T> content = ContentStorage::DragAndDropTargetContent<T>();

	if (content == nullptr) {
		return;
	}

	function(content);
}

////////////////////////////////////////////////////////////////////////////////////////////
// instance
////////////////////////////////////////////////////////////////////////////////////////////

static ContentStorage* const sContentStorage = ContentStorage::GetInstance();

SXAVENGER_ENGINE_NAMESPACE_END
