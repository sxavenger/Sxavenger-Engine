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

	void SetBlases(DxrObject::TopLevelAS* tlas);

	void SetAttributeImGui() override;

private:

	////////////////////////////////////////////////////////////////////////////////////////////
	// MeshStructuredBuffer structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct MeshStructuredBuffer {
		std::unique_ptr<DxObject::StructuredBuffer> index;
		std::unique_ptr<DxObject::StructuredBuffer> vertex;

		void Create(Model* model) {
			index  = std::make_unique<DxObject::StructuredBuffer>(model->GetMeshData(0).indexResource.get());
			vertex = std::make_unique<DxObject::StructuredBuffer>(model->GetMeshData(0).vertexResource.get());
		}
	};

	//=========================================================================================
	// private variables
	//=========================================================================================

	std::list<std::unique_ptr<Subobject>> subobjects_;

	static const std::string filePaths_[SubobjectType::kCountOfSubobjectType];

	std::array<std::unique_ptr<Model>, SubobjectType::kCountOfSubobjectType> models_;
	std::array<MeshStructuredBuffer,   SubobjectType::kCountOfSubobjectType> meshStructuredBuffers_;

};