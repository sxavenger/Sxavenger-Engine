#include "MyMath.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/Logger.h>

////////////////////////////////////////////////////////////////////////////////////////////
// methods
////////////////////////////////////////////////////////////////////////////////////////////

Vector3f CatmullRomInterpolation(const Vector3f& p0, const Vector3f& p1, const Vector3f& p2, const Vector3f& p3, float t) {
	float t2 = t * t;
	float t3 = t2 * t;

	Vector3f e3 = -p0 + (p1 - p2) * 3.0f + p3; // t^3
	Vector3f e2 = p0 * 2.0f - p1 * 5.0f + p2 * 4.0f - p3; // t^2
	Vector3f e1 = -p0 + p2; // t
	Vector3f e0 = p1 * 2.0f;

	return (e3 * t3 + e2 * t2 + e1 * t + e0) * 0.5f;
}

Vector3f CatmullRomPosition(const std::vector<Vector3f>& points, float t) {

	Assert(points.size() >= 4); //!< CatmullRomは4点以上必要

	// 区間数
	size_t division = points.size() - 1;

	// 1区間の長さ
	float areaWidth = 1.0f / division;

	// 区間内の始点を0.0f, 終点を1.0fとしたときの現在の位置
	float t_2 = std::fmod(t, areaWidth) * division;
	// 下限と上限の範囲に収める
	t_2 = std::clamp(t_2, 0.0f, 1.0f);

	// 区間番号
	size_t index = static_cast<size_t>(t / areaWidth);
	size_t zero = 0; //!< size_tの0表記がわからなかったので変数に0を持たせる
	index = std::clamp(index, zero, points.size() - 2);

	size_t index0 = index - 1;
	size_t index1 = index;
	size_t index2 = index + 1;
	size_t index3 = index + 2;

	if (index == 0) {
		index0 = index1;
	}

	if (index3 >= points.size()) {
		index3 = index2;
	}

	const Vector3f& p0 = points[index0];
	const Vector3f& p1 = points[index1];
	const Vector3f& p2 = points[index2];
	const Vector3f& p3 = points[index3];

	return CatmullRomInterpolation(p0, p1, p2, p3, t_2);
}

Vector3f CalculateEuler(const Vector3f& direction) {
	Vector3f result = {};

	// ロール, ピッチ, ロー回転
	result.y = std::atan2(direction.x, direction.z);

	float length = Length({ direction.x, 0.0f, direction.z });
	result.x = std::atan2(-direction.y, length);

	return result;
}
