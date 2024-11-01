#include "Animation.h"
_DXOBJECT_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/Game/SxavengerGame.h>

////////////////////////////////////////////////////////////////////////////////////////////
// Skeleton structure methods
////////////////////////////////////////////////////////////////////////////////////////////

void Skeleton::Update(const Animation& animation, float time) {
	ApplyAnimation(animation, time);
	UpdateMatrix();
}

void Skeleton::UpdateMatrix() {
	// すべてのJointの更新. 親が若いので(indexが子より親の方が小さいので)通常ループで処理可能
	for (auto& joint : joints) {
		// localMatrixの更新
		joint.localMatrix = Matrix::MakeAffine(joint.transform.scale, joint.transform.rotate, joint.transform.translate);

		// skeletonSpaceMatrixの更新
		if (joint.parent) { //!< 親がいる場合
			joint.skeletonSpaceMatrix = joint.localMatrix * joints[*joint.parent].skeletonSpaceMatrix;

		} else {
			//!< 親がいないのでlocalMatrixとskeletonSpaceMatrixは同じにする
			joint.skeletonSpaceMatrix = joint.localMatrix;
		}
	}
}

void Skeleton::ApplyAnimation(const Animation& animation, float time) {

	for (auto& joint : joints) {
		// 対象のJointのAnimationがあれば, 値の適応
		if (auto it = animation.nodeAnimations.find(joint.name); it != animation.nodeAnimations.end()) { //!< animationに対象のJointがある場合
			// animationの参照取得
			const NodeAnimation& nodeAnimation = (*it).second;

			// 値の適応
			joint.transform.scale     = CalculateValue(nodeAnimation.scale, time);
			joint.transform.rotate    = CalculateValue(nodeAnimation.rotate, time);
			joint.transform.translate = CalculateValue(nodeAnimation.translate, time);
		}
	}

}

Vector3f Skeleton::CalculateValue(const std::vector<KeyframeVector3f>& keyframes, float time) {
	Assert(!keyframes.empty()); //!< keyframeが登録されてない

	if (keyframes.size() == 1 || time <= keyframes[0].time) { //!< keyframeが1つ || 現在の時刻がキーフレーム前の場合
		return keyframes[0].value;
	}

	for (size_t i = 0; i < keyframes.size() - 1; ++i) {
		size_t nextIndex = i + 1;

		if (keyframes[i].time <= time && time <= keyframes[nextIndex].time) { //!< timeが範囲内の場合
			// 範囲内でのtを補間
			float t = (time - keyframes[i].time) / (keyframes[nextIndex].time - keyframes[i].time);
			return Lerp(keyframes[i].value, keyframes[nextIndex].value, t);
		}
	}

	// ここまで来た場合は最後のkeyframeより後なので最後の値を返す
	return (*keyframes.rbegin()).value;
}

Quaternion Skeleton::CalculateValue(const std::vector<KeyframeQuaternion>& keyframes, float time) {
	Assert(!keyframes.empty()); //!< keyframeが登録されてない

	if (keyframes.size() == 1 || time <= keyframes[0].time) { //!< keyframeが1つ || 現在の時刻がキーフレーム前の場合
		return keyframes[0].value;
	}

	for (size_t i = 0; i < keyframes.size() - 1; ++i) {
		size_t nextIndex = i + 1;

		if (keyframes[i].time <= time && time <= keyframes[nextIndex].time) { //!< timeが範囲内の場合
			// 範囲内でのtを補間
			float t = (time - keyframes[i].time) / (keyframes[nextIndex].time - keyframes[i].time);
			return Slerp(keyframes[i].value, keyframes[nextIndex].value, t);
		}
	}

	// ここまで来た場合は最後のkeyframeより後なので最後の値を返す
	return (*keyframes.rbegin()).value;
}

////////////////////////////////////////////////////////////////////////////////////////////
// SkinCluster structure
////////////////////////////////////////////////////////////////////////////////////////////

void SkinCluster::Update(const Skeleton& skeleton) {
	for (size_t jointIndex = 0; jointIndex < skeleton.joints.size(); ++jointIndex) {
		Assert(jointIndex < inverseBindPoseMatrices.size());

		Matrix4x4 mat = inverseBindPoseMatrices[jointIndex] * skeleton.joints[jointIndex].skeletonSpaceMatrix;

		(*paletteResource)[static_cast<uint32_t>(jointIndex)].skeletonSpaceMatrix                 = mat;
		(*paletteResource)[static_cast<uint32_t>(jointIndex)].skeletonSpaceInverseTransposeMatrix = mat.Inverse().Transpose();
	}
}

////////////////////////////////////////////////////////////////////////////////////////////
// Animator class methods
////////////////////////////////////////////////////////////////////////////////////////////

void Animator::Create(const Model* model) {
	Assert(model != nullptr, "model is nullptr");

	// 引数の保存
	model_ = model;

	LoadAnimation();
	CreateSkeleton();
	CreateSkinCluster();
	CreateSkinnedBuffer();
}

void Animator::Update(DeltaTimePoint animationTime, uint32_t animationIndex, bool loop) {
	Assert(animationIndex < animations_.size(), "animation index is over.");

	float time = animationTime.time;

	if (loop) {
		time = std::fmod(animationTime.time, animations_[animationIndex].duration);
	}

	skeleton_.Update(animations_[animationIndex], time);

	for (uint32_t index = 0; index < model_->GetMeshSize(); ++index) {

		skinClusters_[index].Update(skeleton_);

		//* skinning
		auto commandList = Sxavenger::GetCommandList();

		//* スキニング *//
		SxavengerGame::SetSkinningPipeline();

		commandList->SetComputeRootShaderResourceView(0, skinClusters_.at(index).paletteResource->GetGPUVirtualAddress());
		commandList->SetComputeRootShaderResourceView(1, model_->GetMesh(index).GetVertexBuffer()->GetGPUVirtualAddress());
		commandList->SetComputeRootShaderResourceView(2, skinClusters_.at(index).influenceResource->GetGPUVirtualAddress());
		commandList->SetComputeRootConstantBufferView(3, skinClusters_.at(index).informationResource->GetGPUVirtualAddress());
		commandList->SetComputeRootUnorderedAccessView(4, skinnedBuffers_.at(index)->GetGPUVirtualAddress());

		commandList->Dispatch(RoundUp((*skinClusters_.at(index).informationResource)[0], 1024), 1, 1);
	}
}

void Animator::BindIABuffer(uint32_t meshIndex) {

	auto commandList = Sxavenger::GetCommandList();

	// animatorの
	D3D12_VERTEX_BUFFER_VIEW vbv = skinnedBuffers_[meshIndex]->GetVertexBufferView();
	D3D12_INDEX_BUFFER_VIEW  ibv = model_->GetMesh(meshIndex).GetIndexBuffer()->GetIndexBufferView();

	commandList->IASetVertexBuffers(0, 1, &vbv);
	commandList->IASetIndexBuffer(&ibv);
}

float Animator::GetAnimationDuration(uint32_t animationIndex) const {
	Assert(animationIndex < animations_.size());
	return animations_.at(animationIndex).duration;
}

void Animator::LoadAnimation() {

	// sceneの取得
	const aiScene* sceneAi = model_->GetScene();
	Assert(sceneAi->HasAnimations()); //!< animationが存在しない

	// サイズの確保
	animations_.resize(sceneAi->mNumAnimations);

	for (uint32_t animationIndex = 0; animationIndex < sceneAi->mNumAnimations; ++animationIndex) {

		// animationの取得
		const aiAnimation* animationAi = sceneAi->mAnimations[animationIndex];

		auto& animation = animations_[animationIndex];

		// animation全体時間の取得
		animation.duration = static_cast<float>(animationAi->mDuration / animationAi->mTicksPerSecond); //!< 時間の単位を秒に変更

		// nodeAnimationの取得
		for (uint32_t channelIndex = 0; channelIndex < animationAi->mNumChannels; ++channelIndex) {

			// assimpではchannelがnodeAnimationを所持しているので取得
			const aiNodeAnim* nodeAnimationAi = animationAi->mChannels[channelIndex];

			// NodeAnimationの参照を取得
			std::string name = nodeAnimationAi->mNodeName.C_Str();
			NodeAnimation& nodeAnimation = animation.nodeAnimations[name];

			// positionKeyの取得
			for (uint32_t keyIndex = 0; keyIndex < nodeAnimationAi->mNumPositionKeys; ++keyIndex) {

				// keyの取得
				aiVectorKey& keyAi = nodeAnimationAi->mPositionKeys[keyIndex];

				// 結果代入先
				KeyframeVector3f keyframe = {};
				keyframe.time  = static_cast<float>(keyAi.mTime / animationAi->mTicksPerSecond); //!< 秒に変更
				keyframe.value = { keyAi.mValue.x, keyAi.mValue.y, -keyAi.mValue.z };            //!< 左手座標系に変換

				// keyframeをnodeAnimationに代入
				nodeAnimation.translate.emplace_back(keyframe);
			}

			// rotate
			for (uint32_t keyIndex = 0; keyIndex < nodeAnimationAi->mNumRotationKeys; ++keyIndex) {

				// keyの取得
				aiQuatKey& keyAi = nodeAnimationAi->mRotationKeys[keyIndex];

				// 結果代入先
				KeyframeQuaternion keyframe = {};
				keyframe.time  = static_cast<float>(keyAi.mTime / animationAi->mTicksPerSecond);       //!< 秒に変更
				keyframe.value = { -keyAi.mValue.x, -keyAi.mValue.y, keyAi.mValue.z, keyAi.mValue.w }; //!< 左手座標系に変換

				// keyframeをnodeAnimationに代入
				nodeAnimation.rotate.emplace_back(keyframe);
			}

			// scale
			for (uint32_t keyIndex = 0; keyIndex < nodeAnimationAi->mNumScalingKeys; ++keyIndex) {

				// keyの取得
				aiVectorKey& keyAi = nodeAnimationAi->mScalingKeys[keyIndex];

				// 結果代入先
				KeyframeVector3f keyframe;
				keyframe.time  = static_cast<float>(keyAi.mTime / animationAi->mTicksPerSecond); //!< 秒に変更
				keyframe.value = { keyAi.mValue.x, keyAi.mValue.y, keyAi.mValue.z };             //!< valueを代入

				// keyframeをnodeAnimationに代入
				nodeAnimation.scale.emplace_back(keyframe);
			}
		}
	}
}

void Animator::CreateSkeleton() {

	skeleton_.root = CreateJoint(model_->GetRootNode(), {}, skeleton_.joints);

	// 名前とindexのmapingを行いアクセスしやすいように
	for (auto& joint : skeleton_.joints) {
		skeleton_.jointMap.emplace(joint.name, joint.index);
	}

	skeleton_.UpdateMatrix(); //!< localMatrixに初期値を入れておく

}

void Animator::CreateSkinCluster() {

	// mesh分の配列数の確保
	skinClusters_.resize(model_->GetMeshSize());

	for (uint32_t index = 0; index < model_->GetMeshSize(); ++index) {

		// 頂点数の出力
		uint32_t vertexNum = static_cast<uint32_t>(model_->GetMesh(index).GetVertexBuffer()->GetIndexSize());

		auto& skinClusterElement = skinClusters_[index];

		// informationResorceの生成
		skinClusterElement.informationResource
			= std::make_unique<DxObject::BufferResource<uint32_t>>(Sxavenger::GetDevicesObj(), 1);
		(*skinClusterElement.informationResource)[0] = vertexNum; //!< vertexNum

		// influenceResourceの生成
		skinClusterElement.influenceResource
			= std::make_unique<DxObject::BufferResource<VertexInfluence>>(Sxavenger::GetDevicesObj(), vertexNum);
		//!< 仮で0番目のmeshから生成
		
		std::memset(skinClusterElement.influenceResource->GetData(), 0, skinClusterElement.influenceResource->GetStructureSize() * skinClusterElement.influenceResource->GetIndexSize()); //!< 0で埋めておく

		// paletteResourceの生成
		skinClusterElement.paletteResource
			= std::make_unique<DxObject::BufferResource<WellForGPU>>(Sxavenger::GetDevicesObj(), static_cast<uint32_t>(skeleton_.joints.size()));
		
		// InverseBindMatrixを格納する場所作成し, 単位行列で埋める
		skinClusterElement.inverseBindPoseMatrices.resize(skeleton_.joints.size());
		std::generate(skinClusterElement.inverseBindPoseMatrices.begin(), skinClusterElement.inverseBindPoseMatrices.end(), Matrix4x4::Identity);

		// ModelのSkinClusterの情報解析
		for (const auto& jointWeight : model_->GetSkinCluster(index)) {
			auto it = skeleton_.jointMap.find(jointWeight.first);

			if (it == skeleton_.jointMap.end()) { //!< この名前のJointは存在しないので
				continue;
			}

			// 該当するindexのinverseBindPoseMatrixを代入
			skinClusterElement.inverseBindPoseMatrices[(*it).second] = jointWeight.second.inverseBindPoseMatrix;

			// 
			for (const auto& vertexWeight : jointWeight.second.vertexWeights) {
				// 該当のvertexIndexのinfluence情報の参照
				auto& currentInfluence = skinClusterElement.influenceResource->operator[](vertexWeight.vertexIndex);

				// 空いてるところに入れる
				for (uint32_t i = 0; i < kNumMaxInfluence; ++i) {
					if (currentInfluence.weights[i] == 0.0f) { //!< weightが空いてる場合
						currentInfluence.weights[i]      = vertexWeight.weight;
						currentInfluence.jointIndices[i] = (*it).second;
						break;
					}
				}
			}
		}
	}
}

void Animator::CreateSkinnedBuffer() {

	skinnedBuffers_.resize(model_->GetMeshSize());

	for (uint32_t i = 0; i < model_->GetMeshSize(); ++i) {
		skinnedBuffers_.at(i)
			= std::make_unique<UnorderedBufferResource<VertexData>>(Sxavenger::GetDevicesObj(), model_->GetMesh(i).GetVertexBuffer()->GetIndexSize());
	}

}

int32_t Animator::CreateJoint(const Node& node, const std::optional<int32_t>& parent, std::vector<Joint>& joints) {
	
	// jointの構築
	Joint joint;
	joint.name                = node.name;
	joint.localMatrix         = node.localMatrix;
	joint.skeletonSpaceMatrix = Matrix4x4::Identity();
	joint.transform           = node.transform;
	joint.index               = static_cast<int32_t>(joints.size()); //!< 現在登録されてるindexに
	joint.parent              = parent;

	joints.emplace_back(joint); //!< skeletonのjoin列に追加

	for (auto& child : node.children) {
		// 子のJointを作成し, そのindexを登録
		int32_t childIndex = CreateJoint(child, joint.index, joints);
		joints[joint.index].children.emplace_back(childIndex);
	}

	// 自身のindexを返す
	return joint.index;

}

////////////////////////////////////////////////////////////////////////////////////////////
// SkinningPipeline class
////////////////////////////////////////////////////////////////////////////////////////////

void SkinningPipeline::Init() {

	std::unique_ptr<CSBlob> blob = std::make_unique<CSBlob>();
	blob->Create(L"animation/skinning.CS.hlsl");

	skinningCS_ = std::make_unique<CSPipeline>();

	CSRootSignatureDesc desc;
	desc.SetVirtualSRV(0, 0);
	desc.SetVirtualSRV(1, 1);
	desc.SetVirtualSRV(2, 2);
	desc.SetCBV(3, 0);
	desc.SetVirtualUAV(4, 0);

	skinningCS_->CreatePipeline(desc, blob.get());
}

void SkinningPipeline::Term() {
	skinningCS_.reset();
}