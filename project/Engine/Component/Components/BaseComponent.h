#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* lib
#include <Lib/Adapter/Json/JsonSerializer.h>

//* c++
#include <concepts>

////////////////////////////////////////////////////////////////////////////////////////////
// forward
////////////////////////////////////////////////////////////////////////////////////////////
class MonoBehaviour;

////////////////////////////////////////////////////////////////////////////////////////////
// BaseComponent class
////////////////////////////////////////////////////////////////////////////////////////////
//! @brief componentの基底クラス
class BaseComponent
	: public IJsonSerializer {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	BaseComponent(MonoBehaviour* behaviour) : behaviour_(behaviour) {}
	virtual ~BaseComponent() = default;

	virtual json PerseToJson() const override { return json(); }

	virtual void InputJson(const json&) override {}

	//* component option *//

	//! @brief inspectorにcomponent固有の情報を表示する
	virtual void ShowComponentInspector() {}

	//* behaviour option *//

	//! @brief componentが所属するbehaviourを取得
	//! @return componentが所属するbehaviourのptr
	MonoBehaviour* GetBehaviour() const;

	//! @brief componentが有効か確認(entityがActiveかどうか)
	bool IsActive() const;

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	MonoBehaviour* behaviour_ = nullptr;

};

////////////////////////////////////////////////////////////////////////////////////////////
// Component concept
////////////////////////////////////////////////////////////////////////////////////////////
template <class T>
concept Component = std::derived_from<T, BaseComponent>;

