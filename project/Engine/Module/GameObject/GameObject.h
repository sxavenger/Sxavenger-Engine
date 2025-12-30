#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/Foundation.h>
#include <Engine/Components/Entity/BehaviourAddress.h>
#include <Engine/Components/Entity/MonoBehaviour.h>	

//* c++
#include <string>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////////////
// GameObject class
////////////////////////////////////////////////////////////////////////////////////////////
//! @brief ゲームオブジェクト基底クラス
class GameObject {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	GameObject(const std::string& name = "GameObject");
	GameObject(BehaviourAddress&& address) : address_(std::move(address)) {}

	virtual ~GameObject();

	//* inspectable option *//

	virtual void Inspectable() {};

	//* operator [access] *//

	MonoBehaviour* operator->() const { return address_.Get(); }

	//* behaviour option *//

	const BehaviourAddress& GetAddress() { return address_; }

	MonoBehaviour* GetBehaviour() const { return address_.Get(); }

protected:

	//=========================================================================================
	// protected variables
	//=========================================================================================

	BehaviourAddress address_ = nullptr;

};

SXAVENGER_ENGINE_NAMESPACE_END
