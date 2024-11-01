#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* geometry
#include <Lib/Geometry/Vector3.h>
#include <Lib/Geometry/Quaternion.h>
#include <Lib/Geometry/Matrix4x4.h>

//* Transform
#include "Transform.h"

//* c++
#include <unordered_map>
#include <vector>
#include <array>

//* model
#include "Model.h"

//* cs
#include <Engine/System/DxObject/DxCSPipeline.h>
#include <Engine/System/DxObject/DxUnorderedBufferResource.h>

//* engine
#include <Engine/System/Performance.h>

////////////////////////////////////////////////////////////////////////////////////////////
// Keyframe structure
////////////////////////////////////////////////////////////////////////////////////////////
template <typename T>
struct Keyframe {
	float time; //!< キーフレーム時刻 * sec
	T value;    //!< キーフレームの値
};

using KeyframeVector3f   = Keyframe<Vector3f>;   //!< scale / translate
using KeyframeQuaternion = Keyframe<Quaternion>; //!< rotation

////////////////////////////////////////////////////////////////////////////////////////////
// Animation structure
////////////////////////////////////////////////////////////////////////////////////////////

template <typename T>
using AnimationCurve = std::vector<Keyframe<T>>;

struct NodeAnimation {
	AnimationCurve<Vector3f>   scale;
	AnimationCurve<Quaternion> rotate;
	AnimationCurve<Vector3f>   translate;
};

struct Animation {
	float duration;                                                //!< アニメーション全体の尺 sec
	std::unordered_map<std::string, NodeAnimation> nodeAnimations; //!< key: node名, value: NodeAnimation
};

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

private:

	static Vector3f CalculateValue(const std::vector<KeyframeVector3f>& keyframes, float time);
	static Quaternion CalculateValue(const std::vector<KeyframeQuaternion>& keyframes, float time);

};


////////////////////////////////////////////////////////////////////////////////////////////
// SkinCluster structure
////////////////////////////////////////////////////////////////////////////////////////////

const uint32_t kNumMaxInfluence = 4;
struct VertexInfluence {
	std::array<float, kNumMaxInfluence> weights;
	std::array<int32_t, kNumMaxInfluence> jointIndices;
};

struct WellForGPU {
	Matrix4x4 skeletonSpaceMatrix;                 //!< position用
	Matrix4x4 skeletonSpaceInverseTransposeMatrix; //!< normal用
};

struct SkinCluster {
	std::vector<Matrix4x4> inverseBindPoseMatrices;

	//* buffers *//
	std::unique_ptr<DxObject::BufferResource<VertexInfluence>> influenceResource;
	std::unique_ptr<DxObject::BufferResource<WellForGPU>> paletteResource;
	std::unique_ptr<DxObject::BufferResource<uint32_t>> informationResource;

	void Update(const Skeleton& skeleton);

};

////////////////////////////////////////////////////////////////////////////////////////////
// Animator class
////////////////////////////////////////////////////////////////////////////////////////////
class Animator {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	Animator() = default;
	Animator(const Model* model) { Create(model); }

	void Create(const Model* model);

	void Update(DeltaTimePoint animationTime, uint32_t animationIndex, bool loop = true);

	void BindIABuffer(uint32_t meshIndex);

	//* getter *//

	const D3D12_VERTEX_BUFFER_VIEW GetVertexBufferView(uint32_t meshIndex) const { return skinnedBuffers_[meshIndex]->GetVertexBufferView(); }

	uint32_t GetAnimationSize() const { return static_cast<uint32_t>(animations_.size()); }

	float GetAnimationDuration(uint32_t animationIndex) const;


private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* external *//

	const Model* model_ = nullptr;

	// TODO: csPipeline

	std::vector<Animation>   animations_; //!< 登録されてるanimation数
	Skeleton                 skeleton_;
	std::vector<SkinCluster> skinClusters_; //!< mesh数

	std::vector<std::unique_ptr<DxObject::UnorderedBufferResource<VertexData>>> skinnedBuffers_;

	//=========================================================================================
	// private methods
	//=========================================================================================

	void LoadAnimation();
	void CreateSkeleton();
	void CreateSkinCluster();
	void CreateSkinnedBuffer();

	static int32_t CreateJoint(const Node& node, const std::optional<int32_t>& parent, std::vector<Joint>& joints);

};

////////////////////////////////////////////////////////////////////////////////////////////
// SkinningPipeline class
////////////////////////////////////////////////////////////////////////////////////////////
class SkinningPipeline {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	void Init();

	void Term();

	void SetPipeline() const { skinningCS_->SetPipeline(); }

	DxObject::CSPipeline* GetPipeline() const { return skinningCS_.get(); }

	//* singleton *//

	SkinningPipeline()  = default;
	~SkinningPipeline() = default;

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	std::unique_ptr<DxObject::CSPipeline> skinningCS_;


};