#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* ucontent
#include "UBaseContent.h"
#include "UContentTexture.h"
#include "UContentModel.h"
#include "UContentAnimation.h"
#include "UContentBlob.h"

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
// UContentStorage class
////////////////////////////////////////////////////////////////////////////////////////////
class UContentStorage {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// using
	////////////////////////////////////////////////////////////////////////////////////////////

	using Storage = std::unordered_map<const std::type_info*, Sxl::OptimizedPathMap<std::shared_ptr<UBaseContent>>>;

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	void Term() { storage_.clear(); }

	//* storage option *//

	template <UContentConcept T>
	std::shared_ptr<T> Import(const std::filesystem::path& filepath, const std::any& param = std::any());

	template <UContentConcept T>
	std::shared_ptr<T> Reload(const std::filesystem::path& filepath, const std::any& param = std::any());

	template <UContentConcept T>
	std::shared_ptr<T> GetContent(const std::filesystem::path& filepath) const;

	std::shared_ptr<UBaseContent> GetContent(const std::type_info* type, const std::filesystem::path& filepath) const;

	const std::type_info* GetType(const std::filesystem::path& filepath) const;
	
	//* singleton *//

	static UContentStorage* GetInstance();

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

	template <UContentConcept T>
	static std::shared_ptr<T> Cast(const std::shared_ptr<UBaseContent>& content);

};

////////////////////////////////////////////////////////////////////////////////////////////
// UContentStorage class template methods
////////////////////////////////////////////////////////////////////////////////////////////

template <UContentConcept T>
inline std::shared_ptr<T> UContentStorage::Import(const std::filesystem::path& filepath, const std::any& param) {
	constexpr const std::type_info* type = &typeid(T);

	if (!Contains(type, filepath)) {
		return this->Reload<T>(filepath, param); //!< Contentが存在しない場合は、読み込みを行う
	}

	return UContentStorage::Cast<T>(storage_[type][filepath]);
}

template <UContentConcept T>
inline std::shared_ptr<T> UContentStorage::Reload(const std::filesystem::path& filepath, const std::any& param) {
	constexpr const std::type_info* type = &typeid(T);

	// contentの作成と読み込み
	std::shared_ptr<UBaseContent> content = std::make_shared<T>();
	content->SetFilepath(filepath);
	content->SetParam(param);
	content->AttachUuid(); //!< uuidの設定

	SxavengerSystem::PushTask(content->GetAsyncExecution(), content);

	// contentをstorageに登録
	storage_[type][filepath] = content;
	return UContentStorage::Cast<T>(content);
}

template <UContentConcept T>
inline std::shared_ptr<T> UContentStorage::GetContent(const std::filesystem::path& filepath) const {
	constexpr const std::type_info* type = &typeid(T);

	if (!Contains(type, filepath)) {
		return nullptr; //!< Contentが存在しない
	}

	return UContentStorage::Cast<T>(storage_.at(type).at(filepath)); //!< Contentを取得
}

template <UContentConcept T>
inline std::shared_ptr<T> UContentStorage::Cast(const std::shared_ptr<UBaseContent>& content) {
	return std::static_pointer_cast<T>(content);
}

////////////////////////////////////////////////////////////////////////////////////////////
// singleton instance variable
////////////////////////////////////////////////////////////////////////////////////////////
static UContentStorage* const sUContentStorage = UContentStorage::GetInstance();
