#include "PrimitiveDrawer.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include <MyEngine.h>

////////////////////////////////////////////////////////////////////////////////////////////
// PrimitiveDrawer class methods
////////////////////////////////////////////////////////////////////////////////////////////

void PrimitiveDrawer::Init() {
	// DrawType line の初期化
	drawDatas_[Line].Create(kMaxLineNum_, 2);
	drawDatas_[Triangle].Create(kMaxTriangleNum_, 3);
}

void PrimitiveDrawer::Term() {
	for (int i = 0; i < DrawType::kCountOfDrawType; ++i) {
		drawDatas_[i].Reset();
	}
}

void PrimitiveDrawer::AllDraw() {
	/*auto commandList = MyEngine::GetCommandList();*/


}

//void PrimitiveDrawer::DrawTriangle(const Vector3f& w1, const Vector3f& w2, const Vector3f& w3, uint32_t color) {
//	// drawDetaに登録
//
//}

PrimitiveDrawer* PrimitiveDrawer::GetInstance() {
	static PrimitiveDrawer instance;
	return &instance;
}
