#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/Content/InputGeometry/InputPrimitiveHelper.h>
#include <Engine/Component/Entity/MonoBehaviour.h>
#include <Engine/Component/Components/Transform/TransformComponent.h>
#include <Engine/Component/Components/MeshRenderer/MeshRendererComponent.h>

////////////////////////////////////////////////////////////////////////////////////////////
// EmissiveActor class
////////////////////////////////////////////////////////////////////////////////////////////
class EmissiveActor
	: public MonoBehaviour {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	void Init();

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	InputMesh mesh_    = CreateCube({ 1.0f, 1.0f, 1.0f });
	Material material_ = CreateMaterial();

	//=========================================================================================
	// private methods
	//=========================================================================================

	static InputMesh CreateCube(const Vector3f& size);
	static Material CreateMaterial();

};
