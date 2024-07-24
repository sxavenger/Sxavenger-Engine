#include "DrawMethod.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
// MyEngine
#include "MyEngine.h" //!< デバイス取り出し

//-----------------------------------------------------------------------------------------
// using
//-----------------------------------------------------------------------------------------
using namespace DxObject;

////////////////////////////////////////////////////////////////////////////////////////////
// DrawMethod namespace methods
////////////////////////////////////////////////////////////////////////////////////////////

DrawData DrawMethods::Sphere(float radius, uint32_t kSubdivision) {
	DrawData result;
	float M_PI = std::numbers::pi_v<float>;

	// vertexResourceの作成
	uint32_t vertexSize = (kSubdivision + 1) * (kSubdivision + 1);
	result.vertex = std::make_unique<DxObject::BufferResource<VertexData>>(MyEngine::GetDevicesObj(), vertexSize);

	const float kLatEvery = M_PI / static_cast<float>(kSubdivision);
	const float kLonEvery = (M_PI * 2.0f) / static_cast<float>(kSubdivision);

	for (uint32_t latIndex = 0; latIndex <= kSubdivision; ++latIndex) {
		float lat = -M_PI / 2.0f + kLatEvery * latIndex;

		for (uint32_t lonIndex = 0; lonIndex <= kSubdivision; ++lonIndex) {
			float lon = lonIndex * kLonEvery;

			// vertexResourceに書き込み
			uint32_t startVertexIndex = (latIndex * (kSubdivision + 1) + lonIndex);

			VertexData point;
			point.position = {
				std::cos(lat) * std::cos(lon) * radius, // x
				std::sin(lat) * radius, // y
				std::cos(lat) * std::sin(lon) * radius, // z
				1.0f,                                   // w
			};
			point.texcoord = {
				static_cast<float>(lonIndex) / static_cast<float>(kSubdivision),
				1.0f - static_cast<float>(latIndex) / static_cast<float>(kSubdivision),
			};
			point.normal = {
				point.position.x,
				point.position.y,
				point.position.z,
			};

			result.vertex->operator[](startVertexIndex) = point;
		}
	}

	// indexResourceの作成
	uint32_t indexSize = (kSubdivision) * (kSubdivision + 1) * 6;
	result.index = std::make_unique<DxObject::IndexBufferResource>(MyEngine::GetDevicesObj(), indexSize);

	for (uint32_t latIndex = 0; latIndex <= kSubdivision; ++latIndex) {
		for (uint32_t lonIndex = 0; lonIndex < kSubdivision; ++lonIndex) {
			// indexResourceに書き込み
			uint32_t currentIndex = (latIndex * kSubdivision + lonIndex) * 6;
			uint32_t startVertexIndex = (latIndex * (kSubdivision + 1) + lonIndex);

			result.index->operator[](currentIndex) = startVertexIndex;                            // pointA
			result.index->operator[](currentIndex + 1) = (startVertexIndex + (kSubdivision + 1)); // pointB
			result.index->operator[](currentIndex + 2) = (startVertexIndex + 1);                  // pointC

			result.index->operator[](currentIndex + 3) = (startVertexIndex + (kSubdivision + 1));     // pointB
			result.index->operator[](currentIndex + 4) = (startVertexIndex + (kSubdivision + 1) + 1); // pointD
			result.index->operator[](currentIndex + 5) = (startVertexIndex + 1);                      // pointC
		}
	}

	return result;
}

DrawData DrawMethods::Plane(const Vector2f& size) {
	DrawData result;

	result.vertex = std::make_unique<BufferResource<VertexData>>(MyEngine::GetDevicesObj(), 4);
	result.vertex->operator[](0).position = { -size.x / 2.0f, -size.y / 2.0f, 0.0f };
	result.vertex->operator[](0).texcoord = { 0.0f, 1.0f };
	result.vertex->operator[](1).position = { size.x / 2.0f, -size.y / 2.0f, 0.0f };
	result.vertex->operator[](1).texcoord = { 1.0f, 1.0f };
	result.vertex->operator[](2).position = { size.x / 2.0f, size.y / 2.0f, 0.0f };
	result.vertex->operator[](2).texcoord = { 1.0f, 0.0f };
	result.vertex->operator[](3).position = { -size.x / 2.0f, size.y / 2.0f, 0.0f };
	result.vertex->operator[](3).texcoord = { 0.0f, 0.0f };


	for (uint32_t i = 0; i < result.vertex->GetIndexSize(); ++i) {
		result.vertex->operator[](i).normal = { 0.0f, 0.0f, 1.0f };
	}

	result.index = std::make_unique<IndexBufferResource>(MyEngine::GetDevicesObj(), 6);
	result.index->operator[](0) = 0;
	result.index->operator[](1) = 3;
	result.index->operator[](2) = 1;
	result.index->operator[](3) = 3;
	result.index->operator[](4) = 2;
	result.index->operator[](5) = 1;

	return result;
}

DrawData DrawMethods::SkyBox(const Vector3f& size) {

	DrawData result;

	result.vertex = std::make_unique<BufferResource<VertexData>>(MyEngine::GetDevicesObj(), 8);

	Vector3f halfSize = size / 2.0f;

	(*result.vertex)[0].position = { -halfSize.x, -halfSize.y, -halfSize.z, };
	(*result.vertex)[1].position = { -halfSize.x, halfSize.y, -halfSize.z, };
	(*result.vertex)[2].position = { -halfSize.x, -halfSize.y, halfSize.z, };
	(*result.vertex)[3].position = { -halfSize.x, halfSize.y, halfSize.z, };
	(*result.vertex)[4].position = { halfSize.x, -halfSize.y, -halfSize.z, };
	(*result.vertex)[5].position = { halfSize.x, halfSize.y, -halfSize.z, };
	(*result.vertex)[6].position = { halfSize.x, -halfSize.y, halfSize.z, };
	(*result.vertex)[7].position = { halfSize.x, halfSize.y, halfSize.z, };

	result.index = std::make_unique<IndexBufferResource>(MyEngine::GetDevicesObj(), 12 * 3);
	(*result.index)[0] = 2; (*result.index)[1] = 0; (*result.index)[2] = 1;
	(*result.index)[3] = 6; (*result.index)[4] = 2; (*result.index)[5] = 3;
	(*result.index)[6] = 4; (*result.index)[7] = 6; (*result.index)[8] = 7;
	(*result.index)[9] = 0; (*result.index)[10] = 4; (*result.index)[11] = 5;
	(*result.index)[12] = 0; (*result.index)[13] = 2; (*result.index)[14] = 6;
	(*result.index)[15] = 5; (*result.index)[16] = 7; (*result.index)[17] = 3;
	(*result.index)[18] = 2; (*result.index)[19] = 1; (*result.index)[20] = 3;
	(*result.index)[21] = 6; (*result.index)[22] = 3; (*result.index)[23] = 7;
	(*result.index)[24] = 4; (*result.index)[25] = 7; (*result.index)[26] = 5;
	(*result.index)[27] = 0; (*result.index)[28] = 5; (*result.index)[29] = 1;
	(*result.index)[30] = 0; (*result.index)[31] = 6; (*result.index)[32] = 4;
	(*result.index)[33] = 5; (*result.index)[34] = 3; (*result.index)[35] = 1;

	return result;
}

DrawData DrawMethods::Plane2D() {
	DrawData result;

	result.vertex = std::make_unique<DxObject::BufferResource<VertexData>>(MyEngine::GetDevicesObj(), 4);
	result.vertex->operator[](0).position = { -1.0f, -1.0f, 0.0f };
	result.vertex->operator[](0).texcoord = { 0.0f, 1.0f };

	result.vertex->operator[](1).position = { -1.0f, 1.0f, 0.0f };
	result.vertex->operator[](1).texcoord = { 0.0f, 0.0f };

	result.vertex->operator[](2).position = { 1.0f, -1.0f, 0.0f };
	result.vertex->operator[](2).texcoord = { 1.0f, 1.0f };

	result.vertex->operator[](3).position = { 1.0f, 1.0f, 0.0f };
	result.vertex->operator[](3).texcoord = { 1.0f, 0.0f };

	result.index = std::make_unique<DxObject::IndexBufferResource>(MyEngine::GetDevicesObj(), 6);
	result.index->operator[](0) = 0;
	result.index->operator[](1) = 1;
	result.index->operator[](2) = 2;
	result.index->operator[](3) = 1;
	result.index->operator[](4) = 3;
	result.index->operator[](5) = 2;

	return result;
}
