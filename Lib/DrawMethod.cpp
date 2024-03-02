#include "DrawMethod.h"

DxObject::BufferResource<VertexData>* DrawMethods::Sphere(float radius, uint32_t kSubdivision) {
	// 要素のサイズを決定 -> x, yともに分割数は共通 * (三角形index * 2)
	uint32_t indexSize = kSubdivision * kSubdivision * 6;
	DxObject::BufferResource<VertexData>* result = new DxObject::BufferResource<VertexData>(MyEngine::GetDevice(), indexSize);

	// dataに書き込み

	const float kLatEvery = static_cast<float>(M_PI) / static_cast<float>(kSubdivision);
	const float kLonEvery = (static_cast<float>(M_PI) * 2.0f) / static_cast<float>(kSubdivision);

	for (uint32_t latIndex = 0; latIndex < kSubdivision; ++latIndex) {
		float lat = -static_cast<float>(M_PI) / 2.0f + kLatEvery * latIndex;

		for (uint32_t lonIndex = 0; lonIndex < kSubdivision; ++lonIndex) {
			float lon = lonIndex * kLonEvery;
			uint32_t startIndex = (latIndex * kSubdivision + lonIndex) * 6;

			VertexData point[4];
			enum POINT {
				A, B, C, D
			};

			// pointA - center
			point[A].position = {
				std::cos(lat) * std::cos(lon) * radius, // x
				std::sin(lat)                 * radius, // y
				std::cos(lat) * std::sin(lon) * radius, // z
				1.0f,                                   // w
			};
			point[A].texcoord = {
				static_cast<float>(lonIndex) / static_cast<float>(kSubdivision),
				1.0f - static_cast<float>(latIndex) / static_cast<float>(kSubdivision),
			};

			// pointB - top
			point[B].position = {
				std::cos(lat + kLatEvery) * std::cos(lon) * radius, // x
				std::sin(lat + kLatEvery)                 * radius, // y
				std::cos(lat + kLatEvery) * std::sin(lon) * radius, // z
				1.0f,                                      // w
			};
			point[B].texcoord = {
				static_cast<float>(lonIndex) / static_cast<float>(kSubdivision),
				1.0f - static_cast<float>(latIndex + 1) / static_cast<float>(kSubdivision),
			};

			// pointC - right
			point[C].position = {
				std::cos(lat) * std::cos(lon + kLonEvery) * radius, // x
				std::sin(lat)                             * radius, // y
				std::cos(lat) * std::sin(lon + kLonEvery) * radius, // z
				1.0f,                                               // w
			};
			point[C].texcoord = {
				static_cast<float>(lonIndex + 1) / static_cast<float>(kSubdivision),
				1.0f - static_cast<float>(latIndex) / static_cast<float>(kSubdivision),
			};

			// pointD - rightTop
			point[D].position = {
				std::cos(lat + kLatEvery) * std::cos(lon + kLonEvery) * radius, // x
				std::sin(lat + kLatEvery)                             * radius, // y
				std::cos(lat + kLatEvery) * std::sin(lon + kLonEvery) * radius, // z
				1.0f,                                                           // w
			};
			point[D].texcoord = {
				static_cast<float>(lonIndex + 1) / static_cast<float>(kSubdivision),
				1.0f - static_cast<float>(latIndex + 1) / static_cast<float>(kSubdivision),
			};

			// resultに書き込み
			result->operator[](startIndex) = point[A];
			result->operator[](startIndex + 1) = point[B];
			result->operator[](startIndex + 2) = point[C];
			result->operator[](startIndex + 3) = point[B];
			result->operator[](startIndex + 4) = point[D];
			result->operator[](startIndex + 5) = point[C];
		}
	}

	return result;
}

DrawData DrawMethods::SphereData(float radius, uint32_t kSubdivision) {
	DrawData result;

	// vertexResourceの作成
	uint32_t vertexSize = (kSubdivision + 1) * (kSubdivision + 1);
	result.vertex = new DxObject::BufferResource<VertexData>(MyEngine::GetDevice(), vertexSize);

	const float kLatEvery = static_cast<float>(M_PI) / static_cast<float>(kSubdivision);
	const float kLonEvery = (static_cast<float>(M_PI) * 2.0f) / static_cast<float>(kSubdivision);

	for (uint32_t latIndex = 0; latIndex <= kSubdivision; ++latIndex) {
		float lat = -static_cast<float>(M_PI) / 2.0f + kLatEvery * latIndex;

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
	result.index = new DxObject::IndexBufferResource(MyEngine::GetDevice(), indexSize);

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
