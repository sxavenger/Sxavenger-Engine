#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* lib
#include <Lib/Adapter/Json/IJsonSerializer.h>

//* c++
#include <concepts>

////////////////////////////////////////////////////////////////////////////////////////////
// forward
////////////////////////////////////////////////////////////////////////////////////////////
class MonoBehaviour;

////////////////////////////////////////////////////////////////////////////////////////////
// BaseComponent class
////////////////////////////////////////////////////////////////////////////////////////////
class BaseComponent
	: public IJsonSerializer {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	BaseComponent(MonoBehaviour* behaviour) : behaviour_(behaviour) {}
	virtual ~BaseComponent() = default;

	virtual void InspectorImGui() {}

	virtual json OutputJson() const override { return json(); }

	virtual void InputJson(const json&) override {}

	//* getter *//

	MonoBehaviour* GetBehaviour() const;

	bool IsActive() const;

	bool IsView() const;

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
