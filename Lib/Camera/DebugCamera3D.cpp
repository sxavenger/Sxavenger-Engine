#include "DebugCamera3D.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////////////////
// DebugCamera3D class methods
////////////////////////////////////////////////////////////////////////////////////////////

void DebugCamera3D::Update() {
	
	if (true) { //!< todo: mouseの中ボタンが押されたときに変更
		if (input_->IsPressKey(DIK_LSHIFT)) {

		} else {

			Vector2i delta = input_->GetDeltaMousePos();

			lon_ = delta.x * deltaRotate;
			lat_ = delta.y * deltaRotate;

		}
	}

}

void DebugCamera3D::SetAttributeImGui() {
}

void DebugCamera3D::CalculateView() {


	
}
