#include "AssetBlob.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/Utility/Logger.h>
#include <Engine/Asset/AssetStorage.h>
#include <Engine/Asset/Observer/AssetObserver.h>

//* external
#include <imgui.h>

//////////////////////////////////////////////////////////////////////////////////////////////
// Asset Blob class methods
//////////////////////////////////////////////////////////////////////////////////////////////

void AssetBlob::Load(_MAYBE_UNUSED const DirectXThreadContext* context) {
	Exception::Assert(param_.has_value(), "profile is not set.");
	DxObject::ShaderBlob::Create(BaseAsset::GetFilepath(), std::any_cast<DxObject::CompileProfile>(param_));
}

void AssetBlob::ShowInspector() {
	BaseAsset::ShowInspector();

	if (ImGui::Button("reload")) {
		sAssetStorage->Import<AssetBlob>(BaseAsset::GetFilepath(), BaseAsset::GetParam());
	}
}
