#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* component
#include "MeshFilterComponent.h"

////////////////////////////////////////////////////////////////////////////////////////////
// MeshFilterComponent class
////////////////////////////////////////////////////////////////////////////////////////////
class MeshFilterContainer final {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// using
	////////////////////////////////////////////////////////////////////////////////////////////

	using Filter = std::unordered_map<InputMesh*, MeshFilterComponent::Container>;

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	MeshFilterComponent::Iterator RegisterFilter(MeshFilterComponent* component);

	void UnregisterFilter(MeshFilterComponent* component);

	//* getter *//

	const Filter& GetFilters() const { return filters_; }

	//* singleton *//

	static MeshFilterContainer* GetInstance();

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	Filter filters_;

};

////////////////////////////////////////////////////////////////////////////////////////////
// singleton
////////////////////////////////////////////////////////////////////////////////////////////
static MeshFilterContainer* sMeshFilterContainer = MeshFilterContainer::GetInstance();
