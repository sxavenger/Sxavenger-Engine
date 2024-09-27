#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* c++
#include <list>
#include <memory>
#include <string>
#include <variant>
#include <optional>

//* system engine
#include <Engine/System/Texture.h>

//* game engine
#include <Engine/Game/Transform.h>
#include <Engine/Game/InputAssembler.h>
#include <Engine/Game/Model.h>

//* lib
#include <Lib/ObjectStructure.h>

////////////////////////////////////////////////////////////////////////////////////////////
// MonoBehavior class
////////////////////////////////////////////////////////////////////////////////////////////
class MonoBehavior {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	MonoBehavior() { transform_.Init(); }
	
	virtual ~MonoBehavior() { Term(); }
	
	//* virtual user methods *//

	virtual void Draw() {}

	virtual void SetMonoBehaviorImGui() {}

	//* behavior option *//

	void SetMonoBehaviorToConsole();
	void SetMonoBehaviorToConsole(const std::string& name);

	//* behavior system methods *//

	void DrawMonoBehavior() {} //!< TODO

	void MonoBehaviorImGui();

	//* behavior accessor *//

	const std::string& GetName() const { return name_; }

	Model* GetIA() const { return ia_; }

	const EulerTransformBuffer& GetTransform() const { return transform_; }
	
protected:

	//=========================================================================================
	// protected variables
	//=========================================================================================

	//* MonoBehavior Info *//

	std::string name_ = "MonoBehavior";

	//* Component *//

	//* transform
	EulerTransformBuffer transform_;
	
	//* IA
	Model* ia_ = nullptr;
	//!< DrawCall情報(singleMesh)
	// TODO: instanceMeshへの対応

	//* material
	// TODO

private:

	//=========================================================================================
	// private methods
	//=========================================================================================

	void Term();

};