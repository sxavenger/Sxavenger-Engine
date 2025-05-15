#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/Component/Entity/MonoBehaviour.h>
#include <Engine/Render/Scene/Environment/FSkyAtmosphere.h>

//* component
#include <Engine/Component/Components/Light/Environment/SkyLightComponent.h>

////////////////////////////////////////////////////////////////////////////////////////////
// AtomosphereActor class
////////////////////////////////////////////////////////////////////////////////////////////
class AtmosphereActor
	: public MonoBehaviour {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	AtmosphereActor() = default;
	~AtmosphereActor() { Term(); }

	void Init(const Vector2f& size);

	void Term();

	void Update();

	void Inspectable() override;

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	FSkyAtmosphere atmosphere_;

	//* components *//

	SkyLightComponent* skyLightComponent_;

};
