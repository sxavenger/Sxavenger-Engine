#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------

//* c++
#include <string>
#include <list>
#include <optional>

////////////////////////////////////////////////////////////////////////////////////////////
// EditorComponent class
////////////////////////////////////////////////////////////////////////////////////////////
class EditorComponent {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// using
	////////////////////////////////////////////////////////////////////////////////////////////
	using Container = std::list<EditorComponent*>;
	using Iterator  = Container::iterator;

public:

	//=========================================================================================
	// public method
	//=========================================================================================

	EditorComponent()  = default;
	~EditorComponent() { Term(); }

	void Term();

	//* component option *//

	void SetName(const std::string& name) { name_ = name; }

	//* container iterator option *//

	void SetChild(EditorComponent* child);

	void SetToConsole();

	void EraseToIteratorContainer();

protected:

	//=========================================================================================
	// protected variables
	//=========================================================================================

	std::string name_ = "new component";

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//!< Containerのiterator, Containerに所属していない場合はnullopt
	//!< 削除用途のために保持している
	std::optional<Iterator> iterator_ = std::nullopt;

	//!< 親コンポーネント
	EditorComponent* parent_ = nullptr;

	//!< 子コンポーネント
	Container children_;

	//=========================================================================================
	// private methods
	//=========================================================================================

	void SetIterator(const Iterator& iterator);

};
