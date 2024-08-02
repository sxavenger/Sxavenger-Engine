#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
// subobject
#include <Subobject.h>

// c++
#include <list>
#include <memory>

// DxObject
#include <DxMesh.h>
#include <DxGraphicsBlob.h>
#include <DxGraphicsPipeline.h>

// IA
#include <Model.h>
#include <DrawMethod.h>

// attribute
#include <Attribute.h>

// engine
#include <MyEngine.h>

// light
#include <Light.h>

////////////////////////////////////////////////////////////////////////////////////////////
// SubobjectType enum class
////////////////////////////////////////////////////////////////////////////////////////////
enum SubobjectType {
	kSprite,
	kPlane,
	kSphere,
	kTeapot,
	kBunny,
	kMultiMesh,
	kMultiMaterial,
	kSuzanne,

	kCountOfSubobjectType
};

////////////////////////////////////////////////////////////////////////////////////////////
// SubobjectPipeline enum class
////////////////////////////////////////////////////////////////////////////////////////////
enum SubobjectPipeline {
	kTextureMesh,
	kNotTextureMesh,

	kCountOfSubobjectPipeline
};


////////////////////////////////////////////////////////////////////////////////////////////
// SubobjectManager class
////////////////////////////////////////////////////////////////////////////////////////////
class SubobjectManager
	: public Attribute {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	SubobjectManager() = default;

	void Init(Camera3D* mainCamera);

	void Term();

	void Update();

	void Draw();

	DxObject::GraphicsPipeline* GetPipeline(SubobjectPipeline pipeline) const { return pipelines_[pipeline].pipeline.get(); }

	const std::string& GetModelDirectory() const { return kModelDirectory_; }

	void SetAttributeImGui() override;

	Camera3D* GetMainCamera() const { return mainCamera_; }

	Light* GetLight() const { return light_.get(); }

private:

	////////////////////////////////////////////////////////////////////////////////////////////
	// GPipeline structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct GPipeline { // FIXME: 名前変更したい
		std::unique_ptr<DxObject::GraphicsBlob>     blob;
		std::unique_ptr<DxObject::GraphicsPipeline> pipeline;

		void Create() {
			blob     = std::make_unique<DxObject::GraphicsBlob>();
			pipeline = std::make_unique<DxObject::GraphicsPipeline>();
		}

		void CreatePipeline() const {
			pipeline->CreatePipeline(MyEngine::GetDevicesObj(), blob.get(), kBlendModeNormal);
		}
	};
	
	//=========================================================================================
	// private variables
	//=========================================================================================

	//* parameter *//

	Camera3D* mainCamera_ = nullptr;

	const std::string kModelDirectory_ = "resources/model/CG2";

	static const std::string kSubobjectNames_[kCountOfSubobjectType];

	//* member *//

	std::list<std::unique_ptr<Subobject>> subobjects_;
	SubobjectType selectedSubobject_ = kPlane;

	std::array<GPipeline, static_cast<int>(SubobjectPipeline::kCountOfSubobjectPipeline)> pipelines_;

	std::unique_ptr<Light> light_;

	//=========================================================================================
	// private methods
	//=========================================================================================

	void CreatePipeline();

};