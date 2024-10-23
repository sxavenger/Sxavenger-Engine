#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/DxObject/DxBufferResource.h>
#include <Engine/System/DxObject/DxShaderBlob.h>
#include <Engine/System/DxObject/DxGraphicsPipeline.h>
#include <Engine/Game/Camera/Camera3D.h>

//* lib
#include <Lib/Geometry/Vector4.h>
#include <Lib/Geometry/Vector3.h>

////////////////////////////////////////////////////////////////////////////////////////////
// BaseDebugPrimitive base class
////////////////////////////////////////////////////////////////////////////////////////////
class BaseDebugPrimitive {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	BaseDebugPrimitive()          = default;
	virtual ~BaseDebugPrimitive() { Term(); }

	void Term();

	void Draw(const Camera3D* camera);

	void Reset();

	void CountBufferOffset();

protected:

	////////////////////////////////////////////////////////////////////////////////////////////
	// PrimitiveInput structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct PrimitiveInput {
		Vector4f position;
		Color4f  color;
	};

	//=========================================================================================
	// protected variables
	//=========================================================================================

	//* buffer *//

	std::unique_ptr<DxObject::BufferResource<PrimitiveInput>> input_;

	//* member *//

	uint32_t inputCount_  = 0; //!< 描画数
	uint32_t inputOffset_ = 0; //!< 前sceneに描画した分

	//=========================================================================================
	// protected methods
	//=========================================================================================

	void CreateInputBuffer(uint32_t bufferSize);

	void SetVertexBuffer(const Vector3f& position, const Color4f& color);

};

////////////////////////////////////////////////////////////////////////////////////////////
// DebugPrimitiveLine class
////////////////////////////////////////////////////////////////////////////////////////////
class DebugPrimitiveLine
	: public BaseDebugPrimitive {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	DebugPrimitiveLine() { Init(); }
	~DebugPrimitiveLine() { Term(); }

	void Init();

	void Term();

	void DrawLine(const Vector3f& v1, const Vector3f& v2, const Color4f& color);

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* config *//

	static const uint32_t kMaxLineNum_ = 1024;
	static const uint32_t kVertexNum_  = 2; //!< lineなので2頂点

};

////////////////////////////////////////////////////////////////////////////////////////////
// DebugPrimitive class
////////////////////////////////////////////////////////////////////////////////////////////
class DebugPrimitive {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	DebugPrimitive()  = default;
	~DebugPrimitive() = default;

	void Init();

	void Term();

	void DrawToScene(const Camera3D* camera);

	void ResetPrimitive();

	void DrawLine(const Vector3f& v1, const Vector3f& v2, const Color4f& color);

	//* drawer options *//

	void DrawGrid();

	void DrawAxis(const Vector3f& center, float length);

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* primitive *//

	std::unique_ptr<DebugPrimitiveLine> line_;

	//* pipeline *//

	std::unique_ptr<DxObject::GraphicsBlob>     blob_;
	std::unique_ptr<DxObject::GraphicsPipeline> pipeline_;

	//* config *//

	static std::wstring directory_;

	//=========================================================================================
	// private methods
	//=========================================================================================

	void CreatePrimitive();
	void CreatePipeline();

};