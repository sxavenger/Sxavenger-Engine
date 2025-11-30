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

//* engine
#include <Engine/System/SxavengerSystem.h>

//* lib
#include <Lib/Sxl/OptimizedPathMap.h>
#include <Lib/Adapter/Uuid/Uuid.h>

//* c++
#include <unordered_map>
#include <memory>
#include <typeinfo>

////////////////////////////////////////////////////////////////////////////////////////////
// ContentStorage class
////////////////////////////////////////////////////////////////////////////////////////////
class ContentStorage {
public:

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
	//! @param[in] param 読み込みパラメータ
	//! @return 読み込んだContentの共有ポインタ
	template <ContentConcept T>
	std::shared_ptr<T> Import(const std::filesystem::path& filepath, const std::any& param = std::any());

	//! @brief Contentの再読み込み
	//! @tparam T Contentの型
	//! @param[in] filepath Contentのファイルパス
	//! @param[in] param 読み込みパラメータ
	//! @return 読み込んだContentの共有ポインタ
	template <ContentConcept T>
	std::shared_ptr<T> Reload(const std::filesystem::path& filepath, const std::any& param = std::any());

	//! @brief Contentの直接登録
	//! @param[in] type Contentの型情報
	//! @param[in] content 登録するContentの共有ポインタ
	void Emplace(const std::type_info* type, const std::shared_ptr<BaseContent>& content);

	//! @brief Contentの登録（存在しない場合のみ）
	//! @param[in] type Contentの型情報
	//! @param[in] content 登録するContentの共有ポインタ
	void TryEmplace(const std::type_info* type, const std::shared_ptr<BaseContent>& content);

	//! @brief Contentの取得
	//! @tparam T Contentの型
	//! @param[in] filepath Contentのファイルパス
	//! @retval ptr     取得したContentの共有ポインタ
	//! @retval nullptr Contentが存在しない場合
	template <ContentConcept T>
	std::shared_ptr<T> GetContent(const std::filesystem::path& filepath) const;

	//! @brief Contentの取得
	//! @param[in] type Contentの型情報
	//! @param[in] filepath Contentのファイルパス
	//! @retval ptr 取得したContentの基底共有ポインタ
	//! @retval nullptr Contentが存在しない場合
	std::shared_ptr<BaseContent> GetContent(const std::type_info* type, const std::filesystem::path& filepath) const;

	//! @brief 指定したfilepathのContent型情報を取得
	//! @param filepath Contentのファイルパス
	//! @retval ptr 取得したContentの型情報
	//! @retval nullptr Contentが存在しない場合
	const std::type_info* GetType(const std::filesystem::path& filepath) const;

	//* imgui option *//

	static void DragAndDropSource(const std::type_info* type, const std::filesystem::path& filepath);

	static std::optional<std::filesystem::path> DragAndDropTargetFilepath(const std::type_info* type);

	template <ContentConcept T>
	std::shared_ptr<T> DragAndDropTargetContent();

	template <ContentConcept T>
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

	bool Contains(const std::type_info* type, const std::filesystem::path& filepath) const;

	template <ContentConcept T>
	static std::shared_ptr<T> Cast(const std::shared_ptr<BaseContent>& content);

};

////////////////////////////////////////////////////////////////////////////////////////////
// ContentStorage class template methods
////////////////////////////////////////////////////////////////////////////////////////////

template <ContentConcept T>
inline std::shared_ptr<T> ContentStorage::Import(const std::filesystem::path& filepath, const std::any& param) {
	constexpr const std::type_info* type = &typeid(T);

	if (!Contains(type, filepath)) {
		return this->Reload<T>(filepath, param); //!< Contentが存在しない場合は、読み込みを行う
	}

	return ContentStorage::Cast<T>(storage_[type][filepath]);
}

template <ContentConcept T>
inline std::shared_ptr<T> ContentStorage::Reload(const std::filesystem::path& filepath, const std::any& param) {
	constexpr const std::type_info* type = &typeid(T);

	std::shared_ptr<BaseContent> content = std::make_shared<T>();
	content->SetFilepath(filepath);
	content->SetParam(param);
	this->Emplace(type, content);

	return ContentStorage::Cast<T>(content);
}

template <ContentConcept T>
inline std::shared_ptr<T> ContentStorage::GetContent(const std::filesystem::path& filepath) const {
	constexpr const std::type_info* type = &typeid(T);

	if (!Contains(type, filepath)) {
		return nullptr; //!< Contentが存在しない
	}

	return ContentStorage::Cast<T>(storage_.at(type).At(filepath)); //!< Contentを取得
}

template <ContentConcept T>
inline std::shared_ptr<T> ContentStorage::DragAndDropTargetContent() {
	constexpr const std::type_info* type = &typeid(T);

	std::optional<std::filesystem::path> filepath = ContentStorage::DragAndDropTargetFilepath(type);

	if (!filepath.has_value()) {
		return nullptr;
	}

	return this->Import<T>(filepath.value());
}

template <ContentConcept T>
inline void ContentStorage::DragAndDropTargetContentFunc(const std::function<void(const std::shared_ptr<T>&)>& function) {
	std::shared_ptr<T> content = this->DragAndDropTargetContent<T>();

	if (content == nullptr) {
		return;
	}

	function(content);
}

template <ContentConcept T>
inline std::shared_ptr<T> ContentStorage::Cast(const std::shared_ptr<BaseContent>& content) {
	return std::static_pointer_cast<T>(content);
}

////////////////////////////////////////////////////////////////////////////////////////////
// singleton instance variable
////////////////////////////////////////////////////////////////////////////////////////////
static ContentStorage* const sContentStorage = ContentStorage::GetInstance();
