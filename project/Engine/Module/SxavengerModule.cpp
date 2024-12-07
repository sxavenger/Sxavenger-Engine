#include "SxavengerModule.h"

////////////////////////////////////////////////////////////////////////////////////////////
// namespace
////////////////////////////////////////////////////////////////////////////////////////////
namespace {
	static std::unique_ptr<DebugPrimitive> sPrimitive = nullptr;
}

////////////////////////////////////////////////////////////////////////////////////////////
// SxavengerModule class methods
////////////////////////////////////////////////////////////////////////////////////////////

void SxavengerModule::Init() {
	sPrimitive = std::make_unique<DebugPrimitive>();
	sPrimitive->Init();
}

void SxavengerModule::Term() {
	sPrimitive.reset();
}

void SxavengerModule::ResetPrimtive() {
	sPrimitive->ResetPrimitive();
}

void SxavengerModule::DrawToScene(const DirectXThreadContext* context, const Camera3d* camera) {
	sPrimitive->DrawToScene(context, camera);
}

void SxavengerModule::DrawLine(const Vector3f& v1, const Vector3f& v2, const Color4f& color) {
	sPrimitive->DrawLine(v1, v2, color);
}

void SxavengerModule::DrawGrid(const Vector3f& center, float size) {
	sPrimitive->DrawGrid(center, size);
}

void SxavengerModule::DrawBox(const Vector3f& min, const Vector3f& max, const Color4f& color) {
	sPrimitive->DrawBox(min, max, color);
}

DebugPrimitive* SxavengerModule::GetDebugPrimitive() {
	return sPrimitive.get();
}
