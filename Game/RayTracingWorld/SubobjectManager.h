#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
// subobject
#include <Subobject.h>

// attribute
#include <Attribute.h>

// c++
#include <list>
#include <memory>
#include <array>

// model
#include <Model.h>

////////////////////////////////////////////////////////////////////////////////////////////
// SubobjectType enum
////////////////////////////////////////////////////////////////////////////////////////////
enum SubobjectType {
	TYPE_CUBE,
	TYPE_TEAPOT,
	TYPE_PLANE,
	TYPE_SPHERE,
	TYPE_SQUARE_PYRAMID,

	kCountOfSubobjectType
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

	SubobjectManager() { Init(); }

	~SubobjectManager() { Term(); }

	void Init();

	void Term();

	void Update();

	void SetBlases(DxrObject::TopLevelAS* tlas);

	void SetAttributeImGui() override;

	void GetBlasModelData(DxrObject::BottomLevelAS* dst, const std::wstring& hitgroupName, SubobjectType type);

private:

	////////////////////////////////////////////////////////////////////////////////////////////
	// MeshStructuredBuffer structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct MeshStructuredBuffer {
		std::unique_ptr<DxObject::StructuredBuffer> index;
		std::unique_ptr<DxObject::StructuredBuffer> vertex;

		void Create(Model* model) {
			index  = std::make_unique<DxObject::StructuredBuffer>(model->GetMesh(0).indexResource.get());
			vertex = std::make_unique<DxObject::StructuredBuffer>(model->GetMesh(0).vertexResource.get());
		}
	};

	//=========================================================================================
	// private variables
	//=========================================================================================

	std::list<std::unique_ptr<Subobject>> subobjects_;

	static const std::string objectName_[SubobjectType::kCountOfSubobjectType];

	std::array<std::unique_ptr<Model>, SubobjectType::kCountOfSubobjectType> models_;
	std::array<MeshStructuredBuffer,   SubobjectType::kCountOfSubobjectType> meshStructuredBuffers_;

	SubobjectType selectedType_ = TYPE_CUBE;

};