#include "PlaneParticle.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include <DirectXCommon.h>
#include <DxDescriptorHeaps.h>
#include <ExecutionSpeed.h>

//=========================================================================================
// static variables
//=========================================================================================
const char* PlaneParticle::item_[] = { "None", "Normal", "Add", "Subtract", "Multiply", "Screen" };

////////////////////////////////////////////////////////////////////////////////////////////
// PlaneParticle methods
////////////////////////////////////////////////////////////////////////////////////////////

void PlaneParticle::Init() {

	// model
	model_ = std::make_unique<Model>("Resources/model", "plane2.obj");

	// material
	materialResource_ = std::make_unique<DxObject::BufferPtrResource<Material>>(MyEngine::GetDevicesObj(), 1);
	materialResource_->Set(0, &material_);

	// particle分の行列を生成
	matrixResource_ = std::make_unique<DxObject::BufferResource<ParticleForGPU>>(MyEngine::GetDevicesObj(), kMaxInstanceNum_);

	for (uint32_t i = 0; i < kMaxInstanceNum_; ++i) {
		matrixResource_->operator[](i).world = Matrix4x4::MakeIdentity();
		matrixResource_->operator[](i).wvp   = Matrix4x4::MakeIdentity();
		matrixResource_->operator[](i).color = { 1.0f, 1.0f, 1.0f, 1.0f };
	}

	// SRVの生成
	{
		// descの設定
		D3D12_SHADER_RESOURCE_VIEW_DESC desc = {};
		desc.Format                     = DXGI_FORMAT_UNKNOWN;
		desc.Shader4ComponentMapping    = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		desc.ViewDimension              = D3D12_SRV_DIMENSION_BUFFER;
		desc.Buffer.FirstElement        = 0;
		desc.Buffer.Flags               = D3D12_BUFFER_SRV_FLAG_NONE;
		desc.Buffer.NumElements         = kMaxInstanceNum_;
		desc.Buffer.StructureByteStride = sizeof(ParticleForGPU);

		// descriptorNumの取得
		kDescriptorNum_
			= MyEngine::GetDxCommon()->GetDescriptorsObj()->GetDescriptorCurrentIndex(DxObject::DescriptorType::SRV);

		// descriptorHandleを生成
		D3D12_CPU_DESCRIPTOR_HANDLE handleCPU
			= MyEngine::GetDxCommon()->GetDescriptorsObj()->GetCPUDescriptorHandle(DxObject::DescriptorType::SRV, kDescriptorNum_);

		handleGPU_
			= MyEngine::GetDxCommon()->GetDescriptorsObj()->GetGPUDescriptorHandle(DxObject::DescriptorType::SRV, kDescriptorNum_);

		MyEngine::GetDevicesObj()->GetDevice()->CreateShaderResourceView(
			matrixResource_->GetResource(),
			&desc,
			handleCPU
		);
	}
}

void PlaneParticle::Update() {

	instanceNum_ = 0;

	for (auto it = elements_.begin(); it != elements_.end();) {

		if (it->currentTime >= it->lifeTime) {
			it = elements_.erase(it); //!< particleの削除
			continue;
		}

		it->transform.translate += it->velocity * ExecutionSpeed::freamsParSec_;
		it->currentTime += ExecutionSpeed::freamsParSec_;

		it->color.a = 1.0f - (it->currentTime / it->lifeTime);

		// resourcesに書き込み
		Matrix4x4 worldMatrix = Matrix4x4::MakeIdentity();
		switch (billBoardType_) {
			case BillBoardType::kBillBoard_NONE:
				worldMatrix = Matrix::MakeAffine(it->transform.scale, it->transform.rotate, it->transform.translate);
				break;

			case BillBoardType::kBillBoard_2D:
				worldMatrix = Matrix::MakeAffine(it->transform.scale, MyEngine::camera3D_->GetCamera().rotate, it->transform.translate);
				break;

			case BillBoardType::kBillBoard_3D:
				worldMatrix = Matrix::MakeAffine(it->transform.scale, it->transform.rotate, it->transform.translate);

				// pi - holizontal
				// fai - vertical
				break;
			
			default:
				assert(false);
				break;
		}

		if (instanceNum_ < kMaxInstanceNum_) { //!< バッファオーバーフロー対策
			matrixResource_->operator[](instanceNum_).world = worldMatrix;
			matrixResource_->operator[](instanceNum_).wvp   = worldMatrix * MyEngine::camera3D_->GetViewProjectionMatrix();
			matrixResource_->operator[](instanceNum_).color = it->color;
			instanceNum_++;
		}

		it++;
	}
}

void PlaneParticle::Draw() {

	MyEngine::SetPipelineType(DxObject::PipelineType::PARTICLE);
	MyEngine::SetBlendMode(blendMode_);
	MyEngine::SetPipelineState();

	// commandListの取り出し
	ID3D12GraphicsCommandList* commandList = MyEngine::GetCommandList();

	for (uint32_t i = 0; i < model_->GetSize(); ++i) {
		model_->SetBuffers(commandList, i);

		commandList->SetGraphicsRootConstantBufferView(0, materialResource_->GetGPUVirtualAddress());
		commandList->SetGraphicsRootDescriptorTable(1, handleGPU_);
		/*model_->SetTexture(2, commandList, 0);*/
		commandList->SetGraphicsRootDescriptorTable(2, MyEngine::GetTextureHandleGPU("resources/particleDemo.png"));


		model_->DrawCall(commandList, i, instanceNum_);
	}
}

void PlaneParticle::Term() {
	model_.reset();
	matrixResource_.reset();

	// descriptorNumを削除
	MyEngine::GetDxCommon()->GetDescriptorsObj()->Erase(DxObject::DescriptorType::SRV, kDescriptorNum_);
}

void PlaneParticle::SetElements(const Element& element) {
	if (elements_.size() >= kMaxInstanceNum_) {
		return;
	}

	elements_.push_back(element);
}

void PlaneParticle::SetOnImGuiEditor() {
	if (ImGui::CollapsingHeader("PlaneParticle")) {

		if (ImGui::BeginCombo("blendMode", item_[blendMode_])) {

			for (int i = 0; i < BlendMode::kCountOfBlendMode; ++i) {
				bool isSelected = (i == blendMode_);
				
				if (ImGui::Selectable(item_[i], isSelected)) {
					blendMode_ = static_cast<BlendMode>(i);
				}
			}

			ImGui::EndCombo();
		}

		SetOnImGuiBillBoardType();

		ImGui::Text("particleNum: %d", elements_.size());
	}
}
