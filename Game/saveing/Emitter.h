#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include <ObjectStructure.h>

#include <memory>

#include <DxBufferResource.h>
#include <Model.h>


//-----------------------------------------------------------------------------------------
// forward
//-----------------------------------------------------------------------------------------
class Particle;

////////////////////////////////////////////////////////////////////////////////////////////
// Emitter class
////////////////////////////////////////////////////////////////////////////////////////////
class Emitter {
public:

	Emitter() { Init(); }

	~Emitter() { Term(); }

	void Init();

	void Update(Particle* particle);

	void Draw();

	void Term();

	void SetOnImGuiEditor();

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	Transform transform_; //!< centerPointの情報
	Vector3f  size_;      //!< パーティクルの出現範囲

	int count_;

	float frequency_;     //!< 発生頻度
	float frequencyTime_; //!< 頻度用時刻

	// draw
	std::unique_ptr<Model> model_;

	std::unique_ptr<DxObject::BufferResource<TransformationMatrix>> matrix_;
	std::unique_ptr<DxObject::BufferResource<Vector4f>>             material_;
};