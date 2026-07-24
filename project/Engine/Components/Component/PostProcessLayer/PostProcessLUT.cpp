#include "PostProcessLUT.h"
SXAVENGER_ENGINE_USING
DXOBJECT_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/UI/SxImGui.h>
#include <Engine/System/System.h>
#include <Engine/Assets/Content/ContentStorage.h>
#include <Engine/Render/Buffer/FRenderTargetBuffer.h>
#include <Engine/Render/Core/FRenderCore.h>
#include <Engine/Render/Core/FRenderCoreProcess.h>

////////////////////////////////////////////////////////////////////////////////////////////
// PostProcessLUT class methods
////////////////////////////////////////////////////////////////////////////////////////////

void PostProcessLUT::Init() {
	name_ = "LUT";

	parameter_ = std::make_unique<ConstantBuffer<Parameter>>();
	parameter_->Create(System::GetDxDevice());
	parameter_->At().intensity = 1.0f;
}

void PostProcessLUT::Process(const DirectXQueueContext* context, const ProcessInfo& info) {
	if (texture_ == nullptr) {
		return; //!< textureが設定されていない場合は処理しない
	}

	FProcessBuffer* process = info.buffer->GetProcess();

	auto core = FRenderCore::GetInstance()->EnsureRenderCore<FRenderCoreProcess>(); //!< RenderCoreの確保.
	core->SetPipeline(FRenderCoreProcess::PostProcess::LUT, context);

	{ //!< LUT

		process->Next();
		process->GetCurrentTexture().TransitionUnorderedAccess(context);

		//!< parameterの設定
		DxObject::BindBufferDesc desc = {};

		//!< 共通parameterの設定
		desc.Set32bitConstants("Dimension", 2,   &info.buffer->GetResolution());
		desc.Set32bitConstants("Information", 1, &info.weight);

		//!< Bufferの設定
		desc.SetHandle("gInput",  process->GetPreviousTexture().GetGPUHandleSRV());
		desc.SetHandle("gOutput", process->GetCurrentTexture().GetGPUHandleUAV());

		core->BindComputeBuffer(FRenderCoreProcess::PostProcess::LUT, context, desc);
		core->Dispatch(context, info.buffer->GetResolution());

		process->GetCurrentTexture().TransitionDefaultState(context);
	}
}

void PostProcessLUT::ShowInspectorImGui() {

	bool isEnable = (texture_ != nullptr);

	{ //!< parameter
		ImGui::BeginDisabled(!isEnable);

		ImGui::DragFloat("intensity", &parameter_->At().intensity, 0.01f, 0.0f, 1.0f);

		ImGui::EndDisabled();
	}

	{ //!< information

		SxImGui::InputScalarN<uint32_t, 2>("tile", &tile_.x);

		ImVec2 target = {};

		if (referenceTexture_.Empty()) {

			Vector2ui size  = { 16, 256 };
			ImVec2 region   = ImGui::GetContentRegionAvail();

			// 画像アス比と分割したWindowアス比の計算
			float textureAspectRatio = static_cast<float>(size.x) / static_cast<float>(size.y);
			float windowAspectRatio  = region.x / region.y;

			// 出力する画像サイズの設定
			ImVec2 displayTextureSize = region;

			// 画像サイズの調整
			if (textureAspectRatio <= windowAspectRatio) {
				displayTextureSize.x *= textureAspectRatio / windowAspectRatio;

			} else {
				displayTextureSize.y *= windowAspectRatio / textureAspectRatio;
			}

			target = displayTextureSize;
			
		} else {

			auto texture = referenceTexture_.WaitGet();

			ImVec2 cursor = ImGui::GetCursorPos();

			ImVec2 region = ImGui::GetContentRegionAvail();
			Vector2f size = static_cast<Vector2f>(texture->GetMetadata().size);

			// 画像アス比と分割したWindowアス比の計算
			float textureAspectRatio = static_cast<float>(size.x) / static_cast<float>(size.y);
			float windowAspectRatio  = region.x / region.y;

			// 出力する画像サイズの設定
			ImVec2 displayTextureSize = region;

			// 画像サイズの調整
			if (textureAspectRatio <= windowAspectRatio) {
				displayTextureSize.x *= textureAspectRatio / windowAspectRatio;

			} else {
				displayTextureSize.y *= windowAspectRatio / textureAspectRatio;
			}

			ImGui::Image(texture->GetGPUHandleSRV().ptr, displayTextureSize);

			target = displayTextureSize;

			ImGui::SetCursorPos(cursor);
		}

		ImGui::InvisibleButton("## lut texture drag and drop", target);

		sContentStorage->DragAndDropTargetContentFunc<ContentTexture>([this](const std::shared_ptr<ContentTexture>& content) {
			content->WaitComplete(); // contentの読み込みを待つ
			CreateTexture(System::GetDirectQueueContext(), content->GetId(), tile_);
		});
	}
}

void PostProcessLUT::CreateTexture(const DirectXQueueContext* context, const AssetParameter<AssetTexture>& texture, const Vector2ui& tile) {

	// 引数の保存
	referenceTexture_ = texture;
	tile_             = tile;

	texture_ = std::make_unique<FLUTTexture>();
	texture_->Create(referenceTexture_.WaitRequire(), tile_);
	texture_->Dispatch(context);
}
