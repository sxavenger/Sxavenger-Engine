#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* base
#include "IScene.h"

//* Game
#include <Game/Player.h>
#include <Game/Enemy.h>

#include <Game/Field.h>
#include <Game/Particle.h>

////////////////////////////////////////////////////////////////////////////////////////////
// Scene_Game class
////////////////////////////////////////////////////////////////////////////////////////////
class Scene_Game
	: public IScene {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	Scene_Game() = default;
	~Scene_Game() override { Term(); }

	void Init() override;

	void Term() override;

	void Update() override;

	void Draw() override;

private:

	//=========================================================================================
	// private methods
	//=========================================================================================

	std::unique_ptr<Player> player_;

	std::unique_ptr<EnemyCollection> enemyCollection_;

	std::unique_ptr<Field> field_;

	std::unique_ptr<ParticleCollection> particle_;

};