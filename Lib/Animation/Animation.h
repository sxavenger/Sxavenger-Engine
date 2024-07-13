#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
// lib
#include <Vector3.h>
#include <Vector4.h>
#include <Quaternion.h>
#include <Matrix4x4.h>

// object
#include <ObjectStructure.h>

// c++
#include <vector>
#include <map>
#include <string>
#include <optional>

////////////////////////////////////////////////////////////////////////////////////////////
// keyframe structure
////////////////////////////////////////////////////////////////////////////////////////////

template <typename T>
struct Keyframe {
	float time; //!< キーフレーム時刻 sec
	T value;    //!< キーフレームの値
};

using KeyframeVector3    = Keyframe<Vector3f>;   //!< scale / translate
using KeyframeQuaternion = Keyframe<Quaternion>; //!< rotate

////////////////////////////////////////////////////////////////////////////////////////////
// Animation structure
////////////////////////////////////////////////////////////////////////////////////////////

//template <typename T>
//struct AnimationCurve {
//	std::vector<Keyframe<T>> keyframes;
//};

template <typename T>
using AnimationCurve = std::vector<Keyframe<T>>;
// 他要素を追加したい場合はstructureのほうを使う

struct NodeAnimation {
	AnimationCurve<Vector3f>   scale;
	AnimationCurve<Quaternion> rotate;
	AnimationCurve<Vector3f>   translate;
};

struct Animation {
	float duration;                                      //!< アニメーション全体の尺 sec
	std::map<std::string, NodeAnimation> nodeAnimations; //!< key: node名, value: NodeAnimation
	// hash mapにしてもいいかも...
};

//!< 仮置きしておく
////////////////////////////////////////////////////////////////////////////////////////////
// Joint structure
////////////////////////////////////////////////////////////////////////////////////////////
struct Joint {
	QuaternionTransform transform;
	Matrix4x4 localMatrix;
	Matrix4x4 skeletonSpaceMatrix; //!< skeleton空間でのmatrix

	std::string name;
	std::vector<int32_t> children; //!< 子のJointのindexリスト, いなければ空
	int32_t index;                 //!< 自身のindex
	std::optional<int32_t> parent; //!< 親のJointのindex, いなければnull
};

////////////////////////////////////////////////////////////////////////////////////////////
// Skeleton structure
////////////////////////////////////////////////////////////////////////////////////////////
struct Skeleton {
	int32_t root; //!< rootJointのindex
	std::map<std::string, int32_t> jointMap; //!< key: joint名, value: index
	//!< hash mapでもいいかも...?
	std::vector<Joint> joints; //!< 所属してるJoint

	//! @brief 自身のjointの更新処理
	void UpdateMatrix();
};

////////////////////////////////////////////////////////////////////////////////////////////
// AnimationMethods namespace
////////////////////////////////////////////////////////////////////////////////////////////
namespace AnimationMethods {

	Vector3f CalculateValue(const std::vector<KeyframeVector3>& keyframes, float time);

	Quaternion CalculateValue(const std::vector<KeyframeQuaternion>& keyframes, float time);

	//!< どこかの構造体に突っ込んでもいいかも...((多分, Skelton構造体に入れる
	void ApplyAnimation(Skeleton& skeleton, const Animation& animation, float time);

}