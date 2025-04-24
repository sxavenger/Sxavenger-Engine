#include "InputPrimitiveHelper.h"

////////////////////////////////////////////////////////////////////////////////////////////
// InputPrimitiveHelper class methods
////////////////////////////////////////////////////////////////////////////////////////////

InputPrimitive InputPrimitiveHelper::CreatePlane(const Vector2f& size) {
	InputPrimitive primitive;

	//!< 4頂点, 2面
	primitive.Create(4, 2);

	auto vertex = primitive.GetVertex();
	auto index  = primitive.GetIndex();

	//!< 頂点の設定
	vertex->At(0).position = { -size.x, 0.0f, size.y, 1.0f };
	vertex->At(0).texcoord = { 0.0f, 0.0f };

	vertex->At(1).position = { size.x, 0.0f, size.y, 1.0f };
	vertex->At(1).texcoord = { 1.0f, 0.0f };

	vertex->At(2).position = { -size.x, 0.0f, -size.y, 1.0f };
	vertex->At(2).texcoord = { 0.0f, 1.0f };
	
	vertex->At(3).position = { size.x, 0.0f, -size.y, 1.0f };
	vertex->At(3).texcoord = { 1.0f, 1.0f };

	for (uint32_t i = 0; i < 4; ++i) {
		vertex->At(i).normal = { 0.0f, 1.0f, 0.0f };
	}

	//!< indexの設定
	index->At(0) = { 0, 1, 2 };
	index->At(1) = { 1, 3, 2 };

	return primitive;
}

InputPrimitive InputPrimitiveHelper::CreatePlaneZForward(const Vector2f& size) {
	InputPrimitive primitive;

	//!< 4頂点, 2面
	primitive.Create(4, 2);

	auto vertex = primitive.GetVertex();
	auto index  = primitive.GetIndex();

	//!< 頂点の設定
	vertex->At(0).position = { -size.x, size.y, 0.0F, 1.0f };
	vertex->At(0).texcoord = { 0.0f, 0.0f };

	vertex->At(1).position = { size.x, size.y, 0.0f, 1.0f };
	vertex->At(1).texcoord = { 1.0f, 0.0f };

	vertex->At(2).position = { -size.x, -size.y, 0.0f, 1.0f };
	vertex->At(2).texcoord = { 0.0f, 1.0f };
	
	vertex->At(3).position = { size.x, -size.y, 0.0f, 1.0f };
	vertex->At(3).texcoord = { 1.0f, 1.0f };

	for (uint32_t i = 0; i < 4; ++i) {
		vertex->At(i).normal = { 0.0f, 0.0f, 1.0f };
	}

	//!< indexの設定
	index->At(0) = { 0, 1, 2 };
	index->At(1) = { 1, 3, 2 };

	return primitive;
}

InputPrimitive InputPrimitiveHelper::CreateCube(const Vector3f& size) {
	InputPrimitive primitive;

	primitive.Create(4 * 6, 2 * 6);

	auto vertex = primitive.GetVertex();
	auto index = primitive.GetIndex();

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

	return primitive;
}
