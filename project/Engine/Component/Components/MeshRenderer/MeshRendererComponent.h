#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* component
#include "../../BaseComponent.h"
#include "../Transform/TransformComponent.h"

//* engine
#include <Engine/Content/InputAssembler/InputMesh.h>
#include <Engine/Content/Material/Material.h>

////////////////////////////////////////////////////////////////////////////////////////////
// MeshRendererComponent class
////////////////////////////////////////////////////////////////////////////////////////////
class MeshRendererComponent
	: public BaseComponent {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	MeshRendererComponent(MonoBehaviour* behaviour) : BaseComponent(behaviour) {}
	~MeshRendererComponent() override = default;

	//* setter *//

	void SetMesh(const InputMesh* mesh) { mesh_ = mesh; }

	void SetMaterial(const Material* material) { material_ = material; }

	//* getter *//

	const InputMesh* GetMesh() const { return mesh_; }

	const Material* GetMaterial() const { return material_; }

	const TransformComponent* GetTransform() const;

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	const InputMesh* mesh_    = nullptr; //!< 仮
	const Material* material_ = nullptr;

};
