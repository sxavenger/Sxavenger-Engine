#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include <Particle.h>

// c++
#include <memory>

// DxObject
#include <DxBufferResource.h>

#include <Model.h>

////////////////////////////////////////////////////////////////////////////////////////////
// PlaneParticle class
////////////////////////////////////////////////////////////////////////////////////////////
class PlaneParticle
	: public Particle {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//! @brief コンストラクタ
	PlaneParticle() { Init(); }

	//! @brief デストラクタ
	~PlaneParticle() { Term(); }

	//! @breif 初期化処理
	void Init() override;

	//! @brief 更新処理
	void Update() override;

	//! @brief 描画処理
	void Draw() override;

	//! @breif 終了処理
	void Term() override;

	//! @brief elementsにelementを設定
	//! 
	//! @param[in] element 設定するelement
	void SetElements(const Element& element) override;

	void SetOnImGuiEditor();

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	static const uint32_t kMaxInstanceNum_ = 100;
	uint32_t              instanceNum_ = 0;

	// model
	std::unique_ptr<Model> model_;

	// material
	Material                                               material_;
	std::unique_ptr<DxObject::BufferPtrResource<Material>> materialResource_;

	// matrix
	std::unique_ptr<DxObject::BufferResource<ParticleForGPU>> matrixResource_;

	uint32_t                    kDescriptorNum_;
	D3D12_GPU_DESCRIPTOR_HANDLE handleGPU_;

	BlendMode blendMode_ = kBlendModeAdd;
	static const char* item_[];

};