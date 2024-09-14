#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
// Graphics
#include <DxShaderBlob.h>
#include <DxGraphicsPipeline.h>

// c++
#include <memory>
#include <vector>
#include <string>

// model
#include <Model.h>

// Deffered
#include <DefferedRendering.h>

// attritbute
#include <Attribute.h>

////////////////////////////////////////////////////////////////////////////////////////////
// NodeModel class
////////////////////////////////////////////////////////////////////////////////////////////
class NodeModel
	: public Attribute {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	NodeModel() = default;

	~NodeModel() { Term(); }

	void Init(DefferedRendering* deffered);

	void Term();

	void Update();

	void Draw();

	void SetAttributeImGui() override;

private:

	////////////////////////////////////////////////////////////////////////////////////////////
	// NodeModelType enum
	////////////////////////////////////////////////////////////////////////////////////////////
	enum NodeModelType {
		NODE_MODEL_01,
		NODE_MODEL_02,
		NODE_MODEL_03,
		NODE_MODEL_08,

		kCountOfNodeModelType
	};


	//=========================================================================================
	// private variables
	//=========================================================================================

	// Graphics
	std::unique_ptr<DxObject::GraphicsBlob>     blob_;
	std::unique_ptr<DxObject::GraphicsPipeline> pipeline_;

	// IA
	std::unique_ptr<Model> model_;

	// ConstantBuffer
	std::vector<std::unique_ptr<DxObject::BufferResource<Matrix4x4>>> matrixBuffers_;

	Matrix4x4 worldMatrix_ = Matrix4x4::Identity();
	EulerTransform transform_;

	// modelType
	NodeModelType type_ = NODE_MODEL_03;
	static const std::string modelNames_[kCountOfNodeModelType];

};