#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include <Collider.h>

// c++
#include <memory>

// DxObject
#include <DxBufferResource.h>

#include <Model.h>
#include <ObjectStructure.h>

//-----------------------------------------------------------------------------------------
// forward
//-----------------------------------------------------------------------------------------
class Particle;

////////////////////////////////////////////////////////////////////////////////////////////
// Field class
////////////////////////////////////////////////////////////////////////////////////////////
class Field {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	Field() { Init(); }

	~Field() { Term(); }

	void Init();

	void Update(Particle* particle);

	void Draw();

	void Term();

	void SetOnImGuiEditor();

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	Vector3f pos_;
	Vector3f size_;
	Vector3f acceleration_;

	// draw
	std::unique_ptr<Model> model_;

	std::unique_ptr<DxObject::BufferResource<TransformationMatrix>> matrix_;
	std::unique_ptr<DxObject::BufferResource<Vector4f>>             material_;

};