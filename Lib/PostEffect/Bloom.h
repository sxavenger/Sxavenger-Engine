#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
// Texture
#include <TextureManager.h>

// DxObject
#include <DxShaderBlob.h>
#include <DxGraphicsPipeline.h>

// DrawMethods
#include <DrawMethod.h>

// Blur
#include <GaussianBlur.h>

#include <Attribute.h>

////////////////////////////////////////////////////////////////////////////////////////////
// Bloom class
////////////////////////////////////////////////////////////////////////////////////////////
class Bloom
	: public Attribute {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	Bloom() { Init(); }

	~Bloom() { Term(); }

	void Init();

	void Term();

	void CreateBloom();

	void Draw();
	
	Texture* GetRenderTargetTexture() const { return renderTargetTexture_.get(); }

	void SetAttributeImGui() override;

private:

	////////////////////////////////////////////////////////////////////////////////////////////
	// Process enum
	////////////////////////////////////////////////////////////////////////////////////////////
	enum Process {
		LUMINNCE,
		BLUR, //!< blurクラスから取得したい
		COMPOSITE, //!< 画像の合成
		COMPOSITE_ADD,
		RESULT,

		kCountOfProcerss
	};
	
	//=========================================================================================
	// private variables
	//=========================================================================================

	/* textures */
	std::unique_ptr<RenderTexture> renderTargetTexture_; //!< HDRレンダリング描画結果書き込み用のTexture
	std::unique_ptr<RenderTexture> luminnceTexture_;     //!< 輝度算出用のTexture
	std::unique_ptr<RenderTexture> luminnceBlurTexture_; //!< 輝度を算出したTextureにブラーをかけるTexture
	//!< 最終な描画結果はユーザー側が指定できるようにする
	
	// SpriteIA
	DrawData sprite_;

	// Graphics
	std::unique_ptr<DxObject::GraphicsBlob>     blob_[kCountOfProcerss];
	std::unique_ptr<DxObject::GraphicsPipeline> pipeline_[kCountOfProcerss];

	std::unique_ptr<GaussianBlur> blur_;

	Vector2ui blurStrength_ = { 24, 8 };
	float sigma_ = 12.0f;

};