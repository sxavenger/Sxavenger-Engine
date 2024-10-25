#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
// DxObject
#include <DxBufferResource.h>
#include <DxShaderBlob.h>
#include <DxGraphicsPipeline.h>

// c++
#include <memory>
#include <string>

// engine
#include <Texture.h>

// geometry
#include <Vector2.h>

// lib
#include <ObjectStructure.h>
#include <Material.h>

// Attribute
#include <Attribute.h>

////////////////////////////////////////////////////////////////////////////////////////////
// Sprite class
////////////////////////////////////////////////////////////////////////////////////////////
class Sprite
	: public Attribute {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	Sprite() = default;

	~Sprite() { Term(); }

	void Init(const std::string& texturefilePath, const Vector2i& pos, const Vector2i& size);

	void Term();

	void Draw();

	void SetAttributeImGui() override;

private:

	////////////////////////////////////////////////////////////////////////////////////////////
	// SpriteMaterial structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct SpriteMaterial {
		Color4f   color;
		Matrix4x4 uvTransform;
		Vector2f  pivot;
		/* pad[2] */

		void Init() {
			color       = { 1.0f, 1.0f, 1.0f, 1.0f };
			uvTransform = Matrix4x4::Identity();
			pivot       = {};
		}
	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// SpriteIA structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct SpriteIA {

		//* variables
		std::unique_ptr<DxObject::BufferResource<VertexData>> vertex; // todo: sprite独自のvertexDataを作る
		std::unique_ptr<DxObject::IndexBufferResource>        index;

		//* methods
		void Create();

		void CalculateVertex(const Vector2i& pos, const Vector2i& size) const;

		void SetIABuffer();

		void DrawCall();

	private:

		//* enum
		enum VertexType {
			LEFTTOP,
			LEFTBOTTOM,
			RIGHTTOP,
			RIGHTBOTTOM
		};

		static Vector4f ToProjection(const Vector2f& screenPostion);

	};

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* member *//
	//* Graphics
	std::unique_ptr<DxObject::GraphicsBlob>     blob_;
	std::unique_ptr<DxObject::GraphicsPipeline> pipeline_;
	//!< TODO: spriteは同じpipelineを使うのでまとめる
	
	//* IA
	SpriteIA ia_;

	//* Buffer *//
	std::unique_ptr<DxObject::BufferResource<SpriteMaterial>> material_;
	UVTransform                                               uvTransform_;
	Texture*                                                  texture_;

	//* data
	Vector2i pos_;
	Vector2i size_;
	std::string filePath_;

	//=========================================================================================
	// private methods
	//=========================================================================================

	void CreatePipeline();


};