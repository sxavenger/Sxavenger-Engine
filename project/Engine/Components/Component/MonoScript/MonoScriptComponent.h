#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* component
#include "../BaseComponent.h"

//* engine
#include <Engine/Foundation.h>
#include <Engine/System/Mono/MonoInstance.h>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////////////
// MonoScriptComponent class
////////////////////////////////////////////////////////////////////////////////////////////
class MonoScriptComponent final
	: public BaseComponent {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	MonoScriptComponent(EntityBehaviour* behaviour);
	~MonoScriptComponent() override;

	//* component option *//

	void ShowComponentInspector() override;

	//* instance option *//

	void Create(const std::string& _namespace, const std::string& _class);

	void Reset();

	void Update();


private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	Mono::Instance instance_;

};

SXAVENGER_ENGINE_NAMESPACE_END
