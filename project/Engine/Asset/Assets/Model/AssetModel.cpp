#include "AssetModel.h"

//////////////////////////////////////////////////////////////////////////////////////////////
// Asset Model class methods
//////////////////////////////////////////////////////////////////////////////////////////////

void AssetModel::Load(const DirectXThreadContext* context) {
	if (BaseAsset::IsLoaded()) {
		return;
	}

	BaseAsset::SetState(BaseAsset::State::Loading);

	Model::Load(context, filepath_, assimpOption_);
	context->ExecuteAllAllocators();

	BaseAsset::SetState(BaseAsset::State::Complete);
}

void AssetModel::ShowInspector() {
}
