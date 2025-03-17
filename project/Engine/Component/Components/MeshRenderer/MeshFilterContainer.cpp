#include "MeshFilterContainer.h"

////////////////////////////////////////////////////////////////////////////////////////////
// MeshFilterComponent class methods
////////////////////////////////////////////////////////////////////////////////////////////

MeshFilterComponent::Iterator MeshFilterContainer::RegisterFilter(MeshFilterComponent* component) {
	//!< filterへの登録
	return filters_[component->GetMesh()].emplace(filters_[component->GetMesh()].end(), component);
}

void MeshFilterContainer::UnregisterFilter(MeshFilterComponent* component) {
	const auto mesh = component->GetMesh();

	//!< filterからの登録解除
	filters_[mesh].erase(component->GetIterator());

	//!< filterの削除
	if (filters_[mesh].empty()) {
		filters_.erase(mesh);
	}
}

MeshFilterContainer* MeshFilterContainer::GetInstance() {
	static MeshFilterContainer instance;
	return &instance;
}
