#include "MeshFilterComponent.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* filter
#include "MeshFilterContainer.h"

//* component
#include "../../MonoBehaviour.h"

////////////////////////////////////////////////////////////////////////////////////////////
// MeshFilterComponent class methods
////////////////////////////////////////////////////////////////////////////////////////////

void MeshFilterComponent::SetMesh(InputMesh* mesh) {
	UnregisterFilter();
	mesh_ = mesh;
	RegisterFilter();
}

const MeshFilterComponent::Iterator& MeshFilterComponent::GetIterator() const {
	Assert(iterator_.has_value(), "iterator is not registered.");
	return iterator_.value();
}

const MeshRendererComponent* MeshFilterComponent::GetRenderer() const {
	return BaseComponent::GetBehaviour()->GetComponent<MeshRendererComponent>();
}

void MeshFilterComponent::RegisterFilter() {
	Assert(!iterator_.has_value(), "iterator is already registered.");
	iterator_ = sMeshFilterContainer->RegisterFilter(this);
}

void MeshFilterComponent::UnregisterFilter() {
	if (!iterator_.has_value()) {
		return;
	}

	sMeshFilterContainer->UnregisterFilter(this);
	iterator_ = std::nullopt;
}
