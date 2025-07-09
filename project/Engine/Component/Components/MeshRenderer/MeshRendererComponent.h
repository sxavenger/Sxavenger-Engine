#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* component
#include "../BaseComponent.h"
#include "../Transform/TransformComponent.h"

//* engine
#include <Engine/Content/InputGeometry/InputMesh.h>
#include <Engine/Content/Material/Material.h>

////////////////////////////////////////////////////////////////////////////////////////////
// MeshRendererComponent class
////////////////////////////////////////////////////////////////////////////////////////////
class MeshRendererComponent final
	: public BaseComponent {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	MeshRendererComponent(MonoBehaviour* behaviour) : BaseComponent(behaviour) {}
	~MeshRendererComponent() override = default;

	void ShowComponentInspector() override;

	//* setter *//

	void SetMesh(InputMesh* mesh) { mesh_ = mesh; }

	void SetMaterial(Material* material) { material_ = material; }

	void SetTransform(const TransformComponent* transform) { transform_ = transform; }

	//* getter *//

	const InputMesh* GetMesh() const { return mesh_; }
	InputMesh* GetMesh() { return mesh_; }

	const Material* GetMaterial() const { return material_; }

	const TransformComponent* GetTransform() const;

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	InputMesh* mesh_    = nullptr;
	Material* material_ = nullptr;

	const TransformComponent* transform_ = nullptr;

};
