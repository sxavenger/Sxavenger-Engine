#include "EmissiveActor.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine


////////////////////////////////////////////////////////////////////////////////////////////
// EmissiveActor class methods
////////////////////////////////////////////////////////////////////////////////////////////

void EmissiveActor::Init() {
	MonoBehaviour::AddComponent<TransformComponent>();

	auto renderer = MonoBehaviour::AddComponent<MeshRendererComponent>();
	renderer->SetMesh(&mesh_);
	renderer->SetMaterial(&material_);
}

InputMesh EmissiveActor::CreateCube(const Vector3f& size) {
	InputMesh mesh;

	mesh.Create(4 * 6, 2 * 6);

	auto vertex = mesh.GetVertex();
	auto index  = mesh.GetIndex();

	Vector3f half = size * 0.5f;

	//!< 頂点の設定
	// 前面 (+z)
	vertex->At(0).position = { -half.x, -half.y,  half.z, 1.0f };
	vertex->At(1).position = { -half.x,  half.y,  half.z, 1.0f };
	vertex->At(2).position = {  half.x,  half.y,  half.z, 1.0f };
	vertex->At(3).position = {  half.x, -half.y,  half.z, 1.0f };

	for (uint32_t i = 0; i < 4; ++i) {
		vertex->At(i).normal = { 0.0f, 0.0f, 1.0f };
	}

	vertex->At(0).texcoord = { 0.0f, 1.0f };
	vertex->At(1).texcoord = { 0.0f, 0.0f };
	vertex->At(2).texcoord = { 1.0f, 0.0f };
	vertex->At(3).texcoord = { 1.0f, 1.0f };

	// 背面 (-z)
	vertex->At(4).position = {  half.x, -half.y, -half.z, 1.0f };
	vertex->At(5).position = {  half.x,  half.y, -half.z, 1.0f };
	vertex->At(6).position = { -half.x,  half.y, -half.z, 1.0f };
	vertex->At(7).position = { -half.x, -half.y, -half.z, 1.0f };

	for (uint32_t i = 4; i < 8; ++i) {
		vertex->At(i).normal = { 0.0f, 0.0f, -1.0f };
	}

	vertex->At(4).texcoord = { 0.0f, 1.0f };
	vertex->At(5).texcoord = { 0.0f, 0.0f };
	vertex->At(6).texcoord = { 1.0f, 0.0f };
	vertex->At(7).texcoord = { 1.0f, 1.0f };

	// 左面 (-x)
	vertex->At(8).position = { -half.x, -half.y, -half.z, 1.0f };
	vertex->At(9).position = { -half.x,  half.y, -half.z, 1.0f };
	vertex->At(10).position = { -half.x,  half.y,  half.z, 1.0f };
	vertex->At(11).position = { -half.x, -half.y,  half.z, 1.0f };

	for (uint32_t i = 8; i < 12; ++i) {
		vertex->At(i).normal = { -1.0f, 0.0f, 0.0f };
	}

	vertex->At(8).texcoord = { 0.0f, 1.0f };
	vertex->At(9).texcoord = { 0.0f, 0.0f };
	vertex->At(10).texcoord = { 1.0f, 0.0f };
	vertex->At(11).texcoord = { 1.0f, 1.0f };

	// 右面 (+x)
	vertex->At(12).position = {  half.x, -half.y,  half.z, 1.0f };
	vertex->At(13).position = {  half.x,  half.y,  half.z, 1.0f };
	vertex->At(14).position = {  half.x,  half.y, -half.z, 1.0f };
	vertex->At(15).position = {  half.x, -half.y, -half.z, 1.0f };

	for (uint32_t i = 12; i < 16; ++i) {
		vertex->At(i).normal = { 1.0f, 0.0f, 0.0f };
	}

	vertex->At(12).texcoord = { 0.0f, 1.0f };
	vertex->At(13).texcoord = { 0.0f, 0.0f };
	vertex->At(14).texcoord = { 1.0f, 0.0f };
	vertex->At(15).texcoord = { 1.0f, 1.0f };

	// 上面 (+y)
	vertex->At(16).position = { -half.x,  half.y,  half.z, 1.0f };
	vertex->At(17).position = { -half.x,  half.y, -half.z, 1.0f };
	vertex->At(18).position = {  half.x,  half.y, -half.z, 1.0f };
	vertex->At(19).position = {  half.x,  half.y,  half.z, 1.0f };

	for (uint32_t i = 16; i < 20; ++i) {
		vertex->At(i).normal = { 0.0f, 1.0f, 0.0f };
	}

	vertex->At(16).texcoord = { 0.0f, 1.0f };
	vertex->At(17).texcoord = { 0.0f, 0.0f };
	vertex->At(18).texcoord = { 1.0f, 0.0f };
	vertex->At(19).texcoord = { 1.0f, 1.0f };

	// 底面 (-y)
	vertex->At(20).position = { -half.x, -half.y, -half.z, 1.0f };
	vertex->At(21).position = { -half.x, -half.y,  half.z, 1.0f };
	vertex->At(22).position = {  half.x, -half.y,  half.z, 1.0f };
	vertex->At(23).position = {  half.x, -half.y, -half.z, 1.0f };

	for (uint32_t i = 20; i < 24; ++i) {
		vertex->At(i).normal = { 0.0f, -1.0f, 0.0f };
	}

	vertex->At(20).texcoord = { 0.0f, 1.0f };
	vertex->At(21).texcoord = { 0.0f, 0.0f };
	vertex->At(22).texcoord = { 1.0f, 0.0f };
	vertex->At(23).texcoord = { 1.0f, 1.0f };

	//!< インデックスの設定 (各面2つの三角形)
	for (int i = 0; i < 6; ++i) {
		int base = i * 4;
		index->At(i * 2)     = { base, base + 1, base + 2 };
		index->At(i * 2 + 1) = { base, base + 2, base + 3 };
	}

	return mesh;
}

Material EmissiveActor::CreateMaterial() {
	Material material;

	material.CreateBuffer();
	material.SetBlendMode(Material::Mode::Emissive);

	return material;
}
