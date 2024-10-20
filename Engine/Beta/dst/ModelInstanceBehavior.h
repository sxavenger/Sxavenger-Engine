#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* base
#include "BaseBehavior.h"

//* engine
#include <Engine/Game/Model.h>
#include <Engine/Game/Transform.h>

////////////////////////////////////////////////////////////////////////////////////////////
// ModelInstanceBehavior class
////////////////////////////////////////////////////////////////////////////////////////////
class ModelInstanceBehavior
	: public BaseBehavior {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	ModelInstanceBehavior() { Init(); }

	~ModelInstanceBehavior() { Term(); }

	void Init();

	void Term();

	//* derivative behavior methods *//
	//* ImGui Commands
	virtual void SystemAttributeImGui() override;
	virtual void SetAttributeImGui() override {} //!< user define

	//* Draw
	virtual void SystemDraw() override;
	virtual void Draw() override {} //!< user define

	//* LocalMesh
	virtual void SystemDrawLocalMesh() override;

	//* option *//

	void CreateInstance(uint32_t instanceSize); //!< user側の呼び出し必須

protected:

	//=========================================================================================
	// protected variables
	//=========================================================================================

	//* member *//

	Model* model_ = nullptr;
	std::unique_ptr<DxObject::BufferResource<TransformationMatrix>> instanceBuffer_;

};