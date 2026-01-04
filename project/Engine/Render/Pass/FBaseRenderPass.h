#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* render
#include "../FRenderTargetBuffer.h"
#include "../Scene/FScene.h"

//* engine
#include <Engine/Foundation.h>
#include <Engine/Components/Component/Camera/CameraComponent.h>

//* c++
#include <concepts>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////////////
// FBaseRenderPass class
////////////////////////////////////////////////////////////////////////////////////////////
class FBaseRenderPass {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// Config structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct Config {
	public:

		////////////////////////////////////////////////////////////////////////////////////////////
		// AntiAliasing enum class
		////////////////////////////////////////////////////////////////////////////////////////////
		enum class AntiAliasing : uint8_t {
			None,
			FXAA,
			SMAA_1x,
		};

		////////////////////////////////////////////////////////////////////////////////////////////
		// Status enum class
		////////////////////////////////////////////////////////////////////////////////////////////
		enum class Status : size_t {
			Success = 0,
			Error_GBuffer   = 1 << 0,
			Warning_Camera  = 1 << 1,
			Warning_Scene   = 1 << 2,

			Geometry_Warning = Warning_Camera | Warning_Scene,
		};

		////////////////////////////////////////////////////////////////////////////////////////////
		// Option enum class
		////////////////////////////////////////////////////////////////////////////////////////////
		enum class Option : size_t {
			IndirectLighting = 1 << 0,
			PostProcess      = 1 << 1,
			Tonemap          = 1 << 2,

			Preview = 1 << 3,

			Default = PostProcess | Tonemap,
		};

	public:

		//=========================================================================================
		// public methods
		//=========================================================================================

		void AttachStatus();
		
		bool CheckStatus(Status status) const;

		//=========================================================================================
		// public variables
		//=========================================================================================

		//* render target buffer *//

		FRenderTargetBuffer* buffer = nullptr;

		//* color space *//

		DxObject::SwapChain::ColorSpace colorSpace = DxObject::SwapChain::ColorSpace::Rec_709;

		//* scene setting *//

		FScene* scene = nullptr;

		//* camera setting *//

		const CameraComponent* camera = nullptr;
		CameraComponent::Tag tag      = CameraComponent::Tag::Game;
		//!< cameraが直接指定されていない場合, tagのcameraを使用する

		const CameraComponent* cullCamera = nullptr; //!< culling用camera
		//!< culling用cameraが直接指定されていない場合, cameraと同じものを使用する

		//* option *//

		AntiAliasing antiAliasing = AntiAliasing::FXAA;
		Sxl::Flag<Option> option  = Option::Default;

	private:

		//=========================================================================================
		// private variables
		//=========================================================================================

		Sxl::Flag<Status> status = Status::Success;

	};

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	FBaseRenderPass()          = default;
	virtual ~FBaseRenderPass() = default;

	virtual void Render(const DirectXQueueContext* context, const Config& config) = 0;

};

////////////////////////////////////////////////////////////////////////////////////////////
// FRenderPass concept
////////////////////////////////////////////////////////////////////////////////////////////
template <class T>
concept FRenderPass = std::derived_from<T, FBaseRenderPass> || !std::is_same_v<T, FBaseRenderPass>;

SXAVENGER_ENGINE_NAMESPACE_END
