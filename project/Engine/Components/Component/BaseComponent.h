#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/Foundation.h>

//* lib
#include <Lib/Adapter/Json/JsonSerializer.h>

//* c++
#include <concepts>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN

//-----------------------------------------------------------------------------------------
// forward
//-----------------------------------------------------------------------------------------
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

	//! @brief behaviourの親を取得
	//! @retval ptr 親
	//! @retval nullptr 親が存在しない
	MonoBehaviour* GetParent() const;

	//! @brief behaviourの親を取得
	//! @return 親のptr
	//! @throw 親が存在しない場合に例外をスローする
	MonoBehaviour* RequireParent() const;

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

SXAVENGER_ENGINE_NAMESPACE_END
