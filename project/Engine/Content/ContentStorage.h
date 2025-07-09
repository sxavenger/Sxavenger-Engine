#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/Utility/Logger.h>

//* lib
#include <Lib/Adapter/Uuid/Uuid.h>

//* c++
#include <unordered_map>
#include <memory>
#include <concepts>
#include <typeindex>

////////////////////////////////////////////////////////////////////////////////////////////
// BaseContent class
////////////////////////////////////////////////////////////////////////////////////////////
class BaseContent { //!< 仮
};
template <class T>
concept ContentConcept = std::derived_from<T, BaseContent>;


////////////////////////////////////////////////////////////////////////////////////////////
// ContentStorage class
////////////////////////////////////////////////////////////////////////////////////////////
class ContentStorage {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// using
	////////////////////////////////////////////////////////////////////////////////////////////
	using Storage = std::unordered_map<const std::type_info*, std::unordered_map<Uuid, std::shared_ptr<BaseContent>>>;

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	template <ContentConcept T>
	void Insert(const Uuid& uuid, const std::shared_ptr<T>& content);

	template <ContentConcept T>
	void Insert(const std::shared_ptr<T>& content);

	template <ContentConcept T>
	std::shared_ptr<T> Get(const Uuid& id) const;

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	Storage storage_;

	//=========================================================================================
	// private methods
	//=========================================================================================

	template <ContentConcept T>
	std::shared_ptr<T> Cast(const std::shared_ptr<BaseContent>& content) const {
		return std::static_pointer_cast<T>(content);
	}

};

////////////////////////////////////////////////////////////////////////////////////////////
// ContentStorage class template methods
////////////////////////////////////////////////////////////////////////////////////////////


