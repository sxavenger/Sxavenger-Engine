#include "Glass.h"

////////////////////////////////////////////////////////////////////////////////////////////
// anonymouse namespace
////////////////////////////////////////////////////////////////////////////////////////////

namespace {

	Vector4f ToColor(uint32_t r, uint32_t g, uint32_t b) {
		Vector4f result;

		result.r = r / 255.0f;
		result.g = g / 255.0f;
		result.b = b / 255.0f;
		result.a = 0.7f;

		return result;
	}

}

////////////////////////////////////////////////////////////////////////////////////////////
// Glass class methods
////////////////////////////////////////////////////////////////////////////////////////////

void Glass::Init() {

	// IA
	model_ = std::make_unique<Model>("./Resources/model", "glass.obj"); // todo: ブロック型のガラスの追加
	modelSize_ = model_->GetSize();

	blasArray_.resize(modelSize_);

	for (uint32_t i = 0; i < modelSize_; ++i) {
		blasArray_[i] = std::make_unique<DxrObject::BottomLevelAS>();
		blasArray_[i]->Create(
			model_->GetMeshData(i).vertexResource.get(), model_->GetMeshData(i).indexResource.get(),
			L"glass"
		);
	}

	// world
	transform_.translate = { 0.0f, 0.0f, -5.0f };
	worldMatrix_ = transform_.SetMatrix();

	// buffer
	colorBuffers_ = std::make_unique<DxObject::BufferResource<Vector4f>>(MyEngine::GetDevicesObj(), modelSize_);
	colorBuffers_->operator[](0) = ToColor(42, 241, 104);
	colorBuffers_->operator[](1) = ToColor(60, 196, 169);
	colorBuffers_->operator[](2) = ToColor(7, 253, 255);
	colorBuffers_->operator[](3) = ToColor(219, 184, 253);
	colorBuffers_->operator[](4) = ToColor(253, 208, 229);
	colorBuffers_->operator[](5) = ToColor(252, 140, 246);
	colorBuffers_->operator[](6) = ToColor(255, 194, 0);
	colorBuffers_->operator[](7) = ToColor(248, 245, 82);

	structuredBuffer_ = std::make_unique<DxObject::StructuredBuffer>(colorBuffers_.get());

}

void Glass::Term() {
}
