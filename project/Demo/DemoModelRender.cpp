#include "DemoModelRender.h"

////////////////////////////////////////////////////////////////////////////////////////////
// DemoModelRender class methods
////////////////////////////////////////////////////////////////////////////////////////////

void DemoModelRender::Init(const std::filesystem::path& filepath, bool isUseMeshShader) {

	model_ = SxavengerAsset::TryImport<Model>(filepath);
	model_.value().Lock()->SetUseMeshShader(isUseMeshShader);
	SxavengerAsset::PushTask(model_.value().Lock());

	//!< default settings
	
	ModelBehavior::SetRenderingFlag(BehaviorRenderingType::kSystematic);
	ModelBehavior::SetName("demo model behavior");

}
