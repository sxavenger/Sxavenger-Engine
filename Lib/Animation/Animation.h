#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
// lib
#include <Vector3.h>
#include <Vector4.h>
#include <Quaternion.h>
#include <Matrix4x4.h>

// engine
#include <Transform.h>

// c++
#include <vector>
#include <unordered_map>
#include <string>
#include <optional>
#include <span>
#include <array>

// DirectX12
#include <d3d12.h>

// DxObject
#include <DxBufferResource.h>

// ComPtr
#include <ComPtr.h>

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
	float duration;                                                //!< アニメーション全体の尺 sec
	std::unordered_map<std::string, NodeAnimation> nodeAnimations; //!< key: node名, value: NodeAnimation
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
	int32_t root;                                      //!< rootJointのindex
	std::unordered_map<std::string, int32_t> jointMap; //!< key: joint名, value: index
	std::vector<Joint> joints;                         //!< 所属してるJoint

	//! @brief animationの適用し, matrixの更新
	void Update(const Animation& animation, float time);

	//! @brief 自身がもってるjointのmatrixの更新処理
	void UpdateMatrix();

	//! @brief 自身にアニメーションを適応
	void ApplyAnimation(const Animation& animation, float time);

};

////////////////////////////////////////////////////////////////////////////////////////////
// AnimationMethods namespace
////////////////////////////////////////////////////////////////////////////////////////////
namespace AnimationMethods {

	Vector3f CalculateValue(const std::vector<KeyframeVector3>& keyframes, float time);

	Quaternion CalculateValue(const std::vector<KeyframeQuaternion>& keyframes, float time);

}

//!< 仮置き
////////////////////////////////////////////////////////////////////////////////////////////
// Weight structure
////////////////////////////////////////////////////////////////////////////////////////////

struct VertexWeightData {
	float weight;
	uint32_t vertexIndex;
};

struct JointWeightData {
	Matrix4x4 inverseBindPoseMatrix;
	std::vector<VertexWeightData> vertexWeights;
};

//!< 仮置き
////////////////////////////////////////////////////////////////////////////////////////////
// SkinCluster structure
////////////////////////////////////////////////////////////////////////////////////////////

const uint32_t kNumMaxInfluence = 4;
struct VertexInfluence {
	std::array<float,   kNumMaxInfluence> weights;
	std::array<int32_t, kNumMaxInfluence> jointIndices;
};

struct WellForGPU {
	Matrix4x4 skeletonSpaceMatrix;                 //!< position用
	Matrix4x4 skeletonSpaceInverseTransposeMatrix; //!< normal用
};

struct SkinCluster {
	std::vector<Matrix4x4> inverseBindPoseMatrices;

	// influence resource
	std::unique_ptr<DxObject::BufferResource<VertexInfluence>> influenceResource;

	// palette resource
	std::unique_ptr<DxObject::BufferResource<WellForGPU>> paletteResource;
	/*DxObject::Descriptor paletteDescriptorSRV;*/

	// infomation resource
	std::unique_ptr<DxObject::BufferResource<uint32_t>> informationResource;

	void Update(const Skeleton& skeleton);

};