#include "Matrix4x4.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* directx
#include <DirectXMath.h>

////////////////////////////////////////////////////////////////////////////////////////////
// define option
////////////////////////////////////////////////////////////////////////////////////////////
#define _USE_DIRECTX_MATH //!< use DirectXMath

////////////////////////////////////////////////////////////////////////////////////////////
// Matrix4x4 class methods
////////////////////////////////////////////////////////////////////////////////////////////

Matrix4x4 Matrix4x4::Transpose() const noexcept {
	Matrix4x4 result = {};

	for (size_t row = 0; row < 4; ++row) {
		for (size_t col = 0; col < 4; ++col) {
			result.m[row][col] = m[col][row];
		}
	}

	return result;
}

Matrix4x4 Matrix4x4::Inverse() const noexcept {

	Matrix4x4 result = {};

#ifdef _USE_DIRECTX_MATH

	DirectX::XMMATRIX xm = DirectX::XMMatrixInverse(nullptr, DirectX::XMLoadFloat4x4(reinterpret_cast<const DirectX::XMFLOAT4X4*>(m.data())));
	DirectX::XMStoreFloat4x4(reinterpret_cast<DirectX::XMFLOAT4X4*>(result.m.data()), xm);

#else

	result.m[0][0] = (m[1][1] * m[2][2] * m[3][3]) + (m[1][2] * m[2][3] * m[3][1]) + (m[1][3] * m[2][1] * m[3][2])
		- (m[1][3] * m[2][2] * m[3][1]) - (m[1][2] * m[2][1] * m[3][3]) - (m[1][1] * m[2][3] * m[3][2]);
	result.m[0][1] = -(m[0][1] * m[2][2] * m[3][3]) - (m[0][2] * m[2][3] * m[3][1]) - (m[0][3] * m[2][1] * m[3][2])
		+ (m[0][3] * m[2][2] * m[3][1]) + (m[0][2] * m[2][1] * m[3][3]) + (m[0][1] * m[2][3] * m[3][2]);
	result.m[0][2] = (m[0][1] * m[1][2] * m[3][3]) + (m[0][2] * m[1][3] * m[3][1]) + (m[0][3] * m[1][1] * m[3][2])
		- (m[0][3] * m[1][2] * m[3][1]) - (m[0][2] * m[1][1] * m[3][3]) - (m[0][1] * m[1][3] * m[3][2]);
	result.m[0][3] = -(m[0][1] * m[1][2] * m[2][3]) - (m[0][2] * m[1][3] * m[2][1]) - (m[0][3] * m[1][1] * m[2][2])
		+ (m[0][3] * m[1][2] * m[2][1]) + (m[0][2] * m[1][1] * m[2][3]) + (m[0][1] * m[1][3] * m[2][2]);

	result.m[1][0] = -(m[1][0] * m[2][2] * m[3][3]) - (m[1][2] * m[2][3] * m[3][0]) - (m[1][3] * m[2][0] * m[3][2])
		+ (m[1][3] * m[2][2] * m[3][0]) + (m[1][2] * m[2][0] * m[3][3]) + (m[1][0] * m[2][3] * m[3][2]);
	result.m[1][1] = (m[0][0] * m[2][2] * m[3][3]) + (m[0][2] * m[2][3] * m[3][0]) + (m[0][3] * m[2][0] * m[3][2])
		- (m[0][3] * m[2][2] * m[3][0]) - (m[0][2] * m[2][0] * m[3][3]) - (m[0][0] * m[2][3] * m[3][2]);
	result.m[1][2] = -(m[0][0] * m[1][2] * m[3][3]) - (m[0][2] * m[1][3] * m[3][0]) - (m[0][3] * m[1][0] * m[3][2])
		+ (m[0][3] * m[1][2] * m[3][0]) + (m[0][2] * m[1][0] * m[3][3]) + (m[0][0] * m[1][3] * m[3][2]);
	result.m[1][3] = (m[0][0] * m[1][2] * m[2][3]) + (m[0][2] * m[1][3] * m[2][0]) + (m[0][3] * m[1][0] * m[2][2])
		- (m[0][3] * m[1][2] * m[2][0]) - (m[0][2] * m[1][0] * m[2][3]) - (m[0][0] * m[1][3] * m[2][2]);

	result.m[2][0] = (m[1][0] * m[2][1] * m[3][3]) + (m[1][1] * m[2][3] * m[3][0]) + (m[1][3] * m[2][0] * m[3][1])
		- (m[1][3] * m[2][1] * m[3][0]) - (m[1][1] * m[2][0] * m[3][3]) - (m[1][0] * m[2][3] * m[3][1]);
	result.m[2][1] = -(m[0][0] * m[2][1] * m[3][3]) - (m[0][1] * m[2][3] * m[3][0]) - (m[0][3] * m[2][0] * m[3][1])
		+ (m[0][3] * m[2][1] * m[3][0]) + (m[0][1] * m[2][0] * m[3][3]) + (m[0][0] * m[2][3] * m[3][1]);
	result.m[2][2] = (m[0][0] * m[1][1] * m[3][3]) + (m[0][1] * m[1][3] * m[3][0]) + (m[0][3] * m[1][0] * m[3][1])
		- (m[0][3] * m[1][1] * m[3][0]) - (m[0][1] * m[1][0] * m[3][3]) - (m[0][0] * m[1][3] * m[3][1]);
	result.m[2][3] = -(m[0][0] * m[1][1] * m[2][3]) - (m[0][1] * m[1][3] * m[2][0]) - (m[0][3] * m[1][0] * m[2][1])
		+ (m[0][3] * m[1][1] * m[2][0]) + (m[0][1] * m[1][0] * m[2][3]) + (m[0][0] * m[1][3] * m[2][1]);

	result.m[3][0] = -(m[1][0] * m[2][1] * m[3][2]) - (m[1][1] * m[2][2] * m[3][0]) - (m[1][2] * m[2][0] * m[3][1])
		+ (m[1][2] * m[2][1] * m[3][0]) + (m[1][1] * m[2][0] * m[3][2]) + (m[1][0] * m[2][2] * m[3][1]);
	result.m[3][1] = (m[0][0] * m[2][1] * m[3][2]) + (m[0][1] * m[2][2] * m[3][0]) + (m[0][2] * m[2][0] * m[3][1])
		- (m[0][2] * m[2][1] * m[3][0]) - (m[0][1] * m[2][0] * m[3][2]) - (m[0][0] * m[2][2] * m[3][1]);
	result.m[3][2] = -(m[0][0] * m[1][1] * m[3][2]) - (m[0][1] * m[1][2] * m[3][0]) - (m[0][2] * m[1][0] * m[3][1])
		+ (m[0][2] * m[1][1] * m[3][0]) + (m[0][1] * m[1][0] * m[3][2]) + (m[0][0] * m[1][2] * m[3][1]);
	result.m[3][3] = (m[0][0] * m[1][1] * m[2][2]) + (m[0][1] * m[1][2] * m[2][0]) + (m[0][2] * m[1][0] * m[2][1])
		- (m[0][2] * m[1][1] * m[2][0]) - (m[0][1] * m[1][0] * m[2][2]) - (m[0][0] * m[1][2] * m[2][1]);

	float denominator = (m[0][0] * m[1][1] * m[2][2] * m[3][3]) + (m[0][0] * m[1][2] * m[2][3] * m[3][1]) + (m[0][0] * m[1][3] * m[2][1] * m[3][2])
		- (m[0][0] * m[1][3] * m[2][2] * m[3][1]) - (m[0][0] * m[1][2] * m[2][1] * m[3][3]) - (m[0][0] * m[1][1] * m[2][3] * m[3][2])
		- (m[0][1] * m[1][0] * m[2][2] * m[3][3]) - (m[0][2] * m[1][0] * m[2][3] * m[3][1]) - (m[0][3] * m[1][0] * m[2][1] * m[3][2])
		+ (m[0][3] * m[1][0] * m[2][2] * m[3][1]) + (m[0][2] * m[1][0] * m[2][1] * m[3][3]) + (m[0][1] * m[1][0] * m[2][3] * m[3][2])
		+ (m[0][1] * m[1][2] * m[2][0] * m[3][3]) + (m[0][2] * m[1][3] * m[2][0] * m[3][1]) + (m[0][3] * m[1][1] * m[2][0] * m[3][2])
		- (m[0][3] * m[1][2] * m[2][0] * m[3][1]) - (m[0][2] * m[1][1] * m[2][0] * m[3][3]) - (m[0][1] * m[1][3] * m[2][0] * m[3][2])
		- (m[0][1] * m[1][2] * m[2][3] * m[3][0]) - (m[0][2] * m[1][3] * m[2][1] * m[3][0]) - (m[0][3] * m[1][1] * m[2][2] * m[3][0])
		+ (m[0][3] * m[1][2] * m[2][1] * m[3][0]) + (m[0][2] * m[1][1] * m[2][3] * m[3][0]) + (m[0][1] * m[1][3] * m[2][2] * m[3][0]);

	if (denominator == 0) { //!< 0除算対策
		return {};
	};

	for (int row = 0; row < 4; row++) {
		for (int column = 0; column < 4; column++) {
			result.m[row][column] /= denominator;
		}
	}

#endif
	return result;
}

Matrix4x4 Matrix4x4::Identity() noexcept {
	return {
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	};
}

Matrix4x4 Matrix4x4::MakeTranslate(const Vector3f& v) noexcept {
	return {
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		v.x, v.y, v.z, 1.0f
	};
}

Matrix4x4 Matrix4x4::MakeRotateX(float radian) noexcept {
	return {
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, std::cos(radian), std::sin(radian), 0.0f,
		0.0f, -std::sin(radian), std::cos(radian), 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	};
}

Matrix4x4 Matrix4x4::MakeRotateY(float radian) noexcept {
	return {
		std::cos(radian), 0.0f, -std::sin(radian), 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		std::sin(radian), 0.0f, std::cos(radian), 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	};
}

Matrix4x4 Matrix4x4::MakeRotateZ(float radian) noexcept {
	return {
		std::cos(radian), std::sin(radian), 0.0f, 0.0f,
		-std::sin(radian), std::cos(radian), 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	};
}

Matrix4x4 Matrix4x4::MakeRotate(const Vector3f& euler) noexcept {
	return MakeRotateX(euler.x) * MakeRotateY(euler.y) * MakeRotateZ(euler.z);
}

Matrix4x4 Matrix4x4::MakeRotate(const Quaternion& q) noexcept {
	return {
		q.w * q.w + q.imaginary.x * q.imaginary.x - q.imaginary.y * q.imaginary.y - q.imaginary.z * q.imaginary.z, 2.0f * (q.imaginary.x * q.imaginary.y + q.w * q.imaginary.z), 2.0f * (q.imaginary.x * q.imaginary.z - q.w * q.imaginary.y), 0.0f,
		2.0f * (q.imaginary.x * q.imaginary.y - q.w * q.imaginary.z), q.w * q.w - q.imaginary.x * q.imaginary.x + q.imaginary.y * q.imaginary.y - q.imaginary.z * q.imaginary.z, 2.0f * (q.imaginary.y * q.imaginary.z + q.w * q.imaginary.x), 0.0f,
		2.0f * (q.imaginary.x * q.imaginary.z + q.w * q.imaginary.y), 2.0f * (q.imaginary.y * q.imaginary.z - q.w * q.imaginary.x), q.w * q.w - q.imaginary.x * q.imaginary.x - q.imaginary.y * q.imaginary.y + q.imaginary.z * q.imaginary.z, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	};
}

Matrix4x4 Matrix4x4::MakeScale(const Vector3f& v) noexcept {
	return {
		v.x, 0.0f, 0.0f, 0.0f,
		0.0f, v.y, 0.0f, 0.0f,
		0.0f, 0.0f, v.z, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	};
}

Matrix4x4 Matrix4x4::MakeAffine(const Vector3f& scale, const Vector3f& rotate, const Vector3f& translate) noexcept {
	return Matrix4x4::MakeScale(scale) * Matrix4x4::MakeRotate(rotate) * Matrix4x4::MakeTranslate(translate);
}

Matrix4x4 Matrix4x4::MakeAffine(const Vector3f& scale, const Quaternion& rotate, const Vector3f& translate) noexcept {
	return Matrix4x4::MakeScale(scale) * Matrix4x4::MakeRotate(rotate) * Matrix4x4::MakeTranslate(translate);
}

Matrix4x4 Matrix4x4::PerspectiveFov(float fov, float aspect, float near, float far) noexcept {
	return {
		1.0f / (aspect * std::tan(fov * 0.5f)), 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f / std::tan(fov * 0.5f), 0.0f, 0.0f,
		0.0f, 0.0f, far / (far - near), 1.0f,
		0.0f, 0.0f, -near * far / (far - near), 0.0f
	};
}

Matrix4x4 Matrix4x4::Orthographic(float left, float top, float right, float bottom, float near, float far) noexcept {
	return {
		2.0f / (right - left), 0.0f, 0.0f, 0.0f,
		0.0f, 2.0f / (top - bottom), 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f / (far - near), 0.0f,
		(left + right) / (left - right), (top + bottom) / (bottom - top), near / (near - far), 1.0f
	};
}

Matrix4x4 Matrix4x4::Viewport(float x, float y, float width, float height, float near, float far) noexcept {
	return {
		width / 2.0f, 0.0f, 0.0f, 0.0f,
		0.0f, -height / 2.0f, 0.0f, 0.0f,
		0.0f, 0.0f, far - near, 0.0f,
		x + (width / 2.0f), y + (height / 2.0f), near, 1.0f,
	};
}

Vector3f Matrix4x4::Transform(const Vector3f& v, const Matrix4x4& m) noexcept {
	Vector3f result = {};

	for (size_t vi = 0; vi < 3; ++vi) {
		result[vi] = v[0] * m.m[0][vi] + v[1] * m.m[1][vi] + v[2] * m.m[2][vi] + m.m[3][vi];
	}

	float w = v[0] * m.m[0][3] + v[1] * m.m[1][3] + v[2] * m.m[2][3] + m.m[3][3];

	if (w == 0.0f) {
		return {};
	}

	return result / w;
}

Vector3f Matrix4x4::TransformNormal(const Vector3f& v, const Matrix4x4& m) noexcept {
	Vector3f result = {};

	for (size_t vi = 0; vi < 3; ++vi) {
		result[vi] = v[0] * m.m[0][vi] + v[1] * m.m[1][vi] + v[2] * m.m[2][vi];
	}

	float w = v[0] * m.m[0][3] + v[1] * m.m[1][3] + v[2] * m.m[2][3];

	if (w == 0.0f) {
		return {};
	}

	return result / w;
}

Vector3f Matrix4x4::GetTranslation(const Matrix4x4& m) noexcept {
	return { m.m[3][0], m.m[3][1], m.m[3][2] };
}
