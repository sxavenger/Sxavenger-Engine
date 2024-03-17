#include "DrawMethod.h"

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
