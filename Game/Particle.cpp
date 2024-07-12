#include "Particle.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include <MyEngine.h>
#include "DxShaderReflection.h"

//-----------------------------------------------------------------------------------------
// using
//-----------------------------------------------------------------------------------------
using namespace DxObject;

//=========================================================================================
// static variables
//=========================================================================================

const std::string Particle::blendModeNames_[kCountOfBlendMode] = {
	"kBlendModeNone",     //!< ブレンドなし
	"kBlendModeNormal",   //!< 通常αブレンド. デフォルト
	"kBlendModeAdd",      //!< 加算
	"kBlendModeSubtract", //!< 減算
	"kBlendModeMultily",  //!< 乗算
	"kBlendModeScreen",   //!< スクリーン
};

////////////////////////////////////////////////////////////////////////////////////////////
// Particle class methods
////////////////////////////////////////////////////////////////////////////////////////////

void Particle::Init(DefferedRendering* defferdRendering) {

	// 引数の保存
	deffered_ = defferdRendering;

	// cs
	{
		csBlob_ = std::make_unique<CSBlob>();
		csBlob_->Init(L"particle.CS.hlsl");

		CSRootSignatureDesc desc;
		desc.Resize(2, 0);
		desc.SetVirtualUAV(0, 0);
		desc.SetCBV(1, 0);

		csPipelineState_ = std::make_unique<CSPipelineState>();
		csPipelineState_->Init(desc, csBlob_.get());

		csBuffer_ = std::make_unique<CSBufferResource<ParticleData>>(MyEngine::GetDevicesObj(), 16 * 16 * 16);
	}

	boolBuffer_ = std::make_unique<BufferPtrResource<int>>(MyEngine::GetDevicesObj(), 1);
	boolBuffer_->SetPtr(0, &isInit_);

	// graphics
	{
		graphicsBlob_ = std::make_unique<GraphicsBlob>();
		graphicsBlob_->Create(L"particle.VS.hlsl", GRAPHICS_VERTEX);
		graphicsBlob_->Create(L"particleDeffered.PS.hlsl", GRAPHICS_PIXEL);

		GraphicsRootSignatureDesc desc;
		desc.Resize(2, 0);
		desc.SetCBV(0, VISIBILITY_VERTEX, 0);
		desc.SetVirtualSRV(1, VISIBILITY_VERTEX, 0);

		pipelineDesc_.CreateDefaultDesc();

		pipelineDesc_.rtvFormats.clear(); //!< RTVFormatの代入し直し
		pipelineDesc_.SetRTVFormats(DefferedRenderingType::kCountOfDefferedRenderingType, deffered_->GetFormats());

		graphicsPipeline_ = std::make_unique<GraphicsPipeline>();
		graphicsPipeline_->CreateRootSignature(MyEngine::GetDevicesObj(), desc);
		graphicsPipeline_->CreatePipeline(MyEngine::GetDevicesObj(), graphicsBlob_.get(), pipelineDesc_);

	}

	// IA
	model_ = std::make_unique<Model>("./Resources/model", "plane.obj");

	SetThisAttribute("particle");
}

void Particle::Term() {
}

void Particle::Update() {

	auto commandList = MyEngine::GetCommandList();

	// update cs
	csPipelineState_->SetCSPipeline();

	commandList->SetComputeRootUnorderedAccessView(0, csBuffer_->GetGPUVirtualAddress());
	commandList->SetComputeRootConstantBufferView(1, boolBuffer_->GetGPUVirtualAddress());

	csPipelineState_->Dispatch(2, 2, 2);

	/*isUpdate = false;*/
}

void Particle::Draw() {

	auto commandList = MyEngine::GetCommandList();

	graphicsPipeline_->SetPipeline(commandList);

	model_->SetBuffers(commandList, 0);

	commandList->SetGraphicsRootConstantBufferView(0, MyEngine::camera3D_->GetGPUVirtualAddress());
	commandList->SetGraphicsRootShaderResourceView(1, csBuffer_->GetGPUVirtualAddress());

	model_->DrawCall(commandList, 0, csBuffer_->GetIndexSize());

}

void Particle::SetAttributeImGui() {

	if (ImGui::BeginCombo("blendMode", blendModeNames_[pipelineDesc_.blendMode].c_str())) {

		for (int i = 0; i < kCountOfBlendMode; ++i) {
			bool isSelect = (i == pipelineDesc_.blendMode);

			if (ImGui::Selectable(blendModeNames_[i].c_str(), isSelect)) {
				pipelineDesc_.blendMode = static_cast<BlendMode>(i);

				if (!isSelect) { //!< 選択されていなかったやつが選択されたので
					graphicsPipeline_->CreatePipeline(MyEngine::GetDevicesObj(), graphicsBlob_.get(), pipelineDesc_);
				}
			}
		}

		ImGui::EndCombo();
	}

	static bool tmp = isInit_;
	ImGui::Checkbox("init", &tmp);
	isInit_ = tmp;

}
