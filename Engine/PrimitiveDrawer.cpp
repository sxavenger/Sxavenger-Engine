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
	drawDatas_[Line].vertices_
		= std::make_unique<DxObject::BufferResource<Vector4f>>(MyEngine::GetDevicesObj(), kMaxLine * 2 /* ラインの頂点数 */);
	drawDatas_[Line].indices_
		= std::make_unique<DxObject::IndexBufferResource>(MyEngine::GetDevicesObj(), kMaxLine * 2 /* ラインの頂点数 */);
	drawDatas_[Line].materials_
		= std::make_unique<DxObject::BufferResource<PrimitiveMaterial>>(MyEngine::GetDevicesObj(), kMaxLine);

	// HACK: make_uniqueする関数を作成

	// matrixResourceの初期化
	matrix_ = std::make_unique<DxObject::BufferResource<PrimitiveMatrix>>(MyEngine::GetDevicesObj(), 1);
}

void PrimitiveDrawer::Term() {
	for (int i = 0; i < DrawType::kCountOfDrawType; ++i) {
		drawDatas_[i].Reset();
	}

	matrix_.reset();
}

void PrimitiveDrawer::AllDraw() {
	// 積まれたバッファの描画
	/*auto commandList = MyEngine::GetCommandList();*/

	// todo:
}

PrimitiveDrawer* PrimitiveDrawer::GetInstance() {
	static PrimitiveDrawer instance;
	return &instance;
}
