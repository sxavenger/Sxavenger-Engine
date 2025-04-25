#include "AssetModel.h"

//////////////////////////////////////////////////////////////////////////////////////////////
// Asset Model class methods
//////////////////////////////////////////////////////////////////////////////////////////////

void AssetModel::Load(const DirectXThreadContext* context) {

	uint32_t option = Model::GetDefaultAssimpOption();

	if (param_.has_value()) {
		param_ = std::any_cast<uint32_t>(param_);
	}

	Model::Load(context, filepath_, option);
}

void AssetModel::ShowInspector() {
}
