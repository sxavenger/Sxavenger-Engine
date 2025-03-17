#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* component
#include "../../BaseComponent.h"
#include "MeshRendererComponent.h"

//* engine
#include <Engine/Content/InputAssembler/InputMesh.h>

////////////////////////////////////////////////////////////////////////////////////////////
// MeshFilterComponent class
////////////////////////////////////////////////////////////////////////////////////////////
class MeshFilterComponent final
	: public BaseComponent {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// using
	////////////////////////////////////////////////////////////////////////////////////////////

	using Container = std::list<MeshFilterComponent*>;
	using Iterator  = Container::iterator;

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	MeshFilterComponent(MonoBehaviour* behaviour) : BaseComponent(behaviour) {}
	~MeshFilterComponent() override = default;

	//* mesh option *//

	void SetMesh(InputMesh* mesh);

	InputMesh* GetMesh() const { return mesh_; }

	//* iterator option *//

	const Iterator& GetIterator() const;

	//* other component option *//

	const MeshRendererComponent* GetRenderer() const;

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* mesh *//

	InputMesh* mesh_ = nullptr;

	//* iterator *//

	std::optional<Iterator> iterator_ = std::nullopt;

	//=========================================================================================
	// private methods
	//=========================================================================================

	void RegisterFilter();

	void UnregisterFilter();

};
