#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* base
#include <Engine/Game/Behavior/ModelBehavior.h>
#include <Engine/System/Texture.h>
#include <Engine/System/Performance.h>
#include <Engine/Console/SystemConsole.h>

//* c++
#include <cstdint>

////////////////////////////////////////////////////////////////////////////////////////////
// Score class
////////////////////////////////////////////////////////////////////////////////////////////
class Score
	: public ModelBehavior {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	void Init();

	void Term();

	void Update();

	void SetAttributeImGui() override;

	void DrawAdaptive(_MAYBE_UNUSED const Camera3D* camera);

	void AddScore(uint32_t addScore, Model* destroyEnemyModel);

	//=========================================================================================
	// public variables
	//=========================================================================================
	
private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* external *//

	CineCamera* camera_ = sSystemConsole->GetGameCamera();

	uint32_t score = 0;

	Texture* texture_;
	Vector2f position_ = { 800.0f, 480.0f };

	DeltaTimePoint destroyTimer_;
	Vector2ui displayDestroyModelPos_ = { 100, 100 };

	Texture* boardTexture_;

	//=========================================================================================
	// private methods
	//=========================================================================================

	Vector2f ToProjection();

};