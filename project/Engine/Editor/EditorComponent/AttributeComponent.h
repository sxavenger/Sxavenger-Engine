#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* c++
#include <list>
#include <string>
#include <functional>
#include <optional>

////////////////////////////////////////////////////////////////////////////////////////////
// AttributeComponent class
////////////////////////////////////////////////////////////////////////////////////////////
class AttributeComponent {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// using
	////////////////////////////////////////////////////////////////////////////////////////////

	using Container = std::list<AttributeComponent*>;
	using Iterator  = Container::const_iterator;

	////////////////////////////////////////////////////////////////////////////////////////////
	// Status enum class
	////////////////////////////////////////////////////////////////////////////////////////////
	enum class Status : uint8_t {
		Unregistered, //!< 未登録
		Child,        //!< childとして設定
		Outliner      //!< outlinerとして設定
	};

public:

	//=========================================================================================
	// public method
	//=========================================================================================

	AttributeComponent()          = default;
	virtual ~AttributeComponent() { RemoveIterator(); }

	void ExecuteAttribute() { attributeFunc_(); }

	//* attribute option *//

	virtual void AttributeImGui() {}

	//* component option *//

	void SetToOutliner();

	void SetChild(AttributeComponent* child);

	void RemoveIterator();

	//* getter *//

	const Container& GetChildren() const { return children_; }

	const std::string& GetName() const { return name_; }
	std::string& GetName() { return name_; }

protected:

	//=========================================================================================
	// protected variables
	//=========================================================================================

	std::string name_ = "new attribute";

	std::function<void()> attributeFunc_ = [this]() { AttributeImGui(); };

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	std::optional<Iterator> iterator_;

	AttributeComponent* parent_ = nullptr;
	Container           children_;

	Status GetStatus() const;
	bool CheckIteratorEmpty() const;

};
