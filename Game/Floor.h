#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
// DxObject
#include <DxBufferResource.h>
#include <DxStructuredBuffer.h>

// c++
#include <cstdint>
#include <memory>

#include <ObjectStructure.h>

#include <Model.h>

#include <Attribute.h>

////////////////////////////////////////////////////////////////////////////////////////////
// Floor class
////////////////////////////////////////////////////////////////////////////////////////////
class Floor
	: public Attribute {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================
	
	//! @brief コンストラクタ
	Floor() { Init(); }

	//! @brief デストラクタ
	~Floor() { Term(); }

	void Init();

	void Update();

	void Draw();

	void Term();

	void SetAttributeImGui() override;

private:

	////////////////////////////////////////////////////////////////////////////////////////////
	// VertexType enum
	////////////////////////////////////////////////////////////////////////////////////////////
	enum VertexType {
		LEFTBOTTOM,
		LEFTTOP,
		RIGHTBOTTOM,
		RIGHTTOP,

		kCountOfVertexType
	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// FloorMaterial structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct FloorMaterial {
		Vector4f color;
	};

	//=========================================================================================
	// private variables
	//=========================================================================================

	// vertexResource
	std::unique_ptr<DxObject::BufferPtrResource<VertexData>> vertexResource_;
	VertexData vertices_[kCountOfVertexType];
	std::unique_ptr<DxObject::StructuredBuffer> vertexStructuredBuffer_;

	// indexResource
	std::unique_ptr<DxObject::IndexBufferResource> indexResource_;
	std::unique_ptr<DxObject::StructuredBuffer> indexStructuredBuffer_;

	// materialResource
	std::unique_ptr<DxObject::BufferPtrResource<FloorMaterial>> materialResource_;
	FloorMaterial material_;

	std::unique_ptr<DxObject::BufferResource<TransformationMatrix>> matrixResource_;
	Transform transform_;

	// parameter //
	Vector2f floorSize_ = { 10.0f, 10.0f };
	float tileScale = 1.0f;

};