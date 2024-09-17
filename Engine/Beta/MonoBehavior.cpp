#include "MonoBehavior.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/Sxavenger.h>

//* lib
#include <Lib/Environment.h>

//* beta
#include "BetaConsole.h"

////////////////////////////////////////////////////////////////////////////////////////////
// MonoBehavior class methods
////////////////////////////////////////////////////////////////////////////////////////////

void MonoBehavior::SetMonoBehaviorToConsole() {
	sBetaConsole->SetMonoBehavior(this);
}

void MonoBehavior::SetMonoBehaviorToConsole(const std::string& name) {
	name_ = name;
	SetMonoBehaviorToConsole();
}

void MonoBehavior::MonoBehaviorImGui() {
	if (ia_ != nullptr) {
		if (ImGui::TreeNode("IA")) {

			ImVec2 windowPadding = ImGui::GetStyle().WindowPadding;

			// タブ等を排除した全体のwindowSize計算
			ImVec2 regionMax = ImGui::GetWindowContentRegionMax();
			ImVec2 regionMin = ImGui::GetWindowContentRegionMin();
			ImVec2 windowSize = { regionMax.x - regionMin.x - windowPadding.x * 4.0f, regionMax.y - regionMin.y};

			Vector2f textureSize = sBetaConsole->GetLocalRenderTexture()->GetSize();

			// 画像アス比と分割したWindowアス比の計算
			float textureAspectRatio = textureSize.x / textureSize.y;
			float windowAspectRatio = windowSize.x / windowSize.y;

			// 出力する画像サイズの設定
			ImVec2 displayTextureSize = windowSize;

			// 画像サイズの調整
			if (textureAspectRatio <= windowAspectRatio) {
				displayTextureSize.x *= textureAspectRatio / windowAspectRatio;

			} else {
				displayTextureSize.y *= windowAspectRatio / textureAspectRatio;
			}

			ImGui::Image((ImTextureID)sBetaConsole->GetLocalRenderTexture()->GetGPUHandleSRV().ptr, displayTextureSize);

			ImGui::TreePop();
		}
	}

	if (ImGui::TreeNode("transform")) {
		transform_.SetImGuiCommand();
		ImGui::TreePop();
	}
}

void MonoBehavior::Term() {
	sBetaConsole->RemoveMonoBehavior(this);
}