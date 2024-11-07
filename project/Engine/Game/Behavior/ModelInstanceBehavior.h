//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* base
#include "ModelBehavior.h"

//* engine
#include <Engine/Game/Model.h>
#include <Engine/Game/Transform.h>

////////////////////////////////////////////////////////////////////////////////////////////
// ModelInstanceBehavior class
////////////////////////////////////////////////////////////////////////////////////////////
class ModelInstanceBehavior
	: public ModelBehavior {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	ModelInstanceBehavior() { Init(); }

	~ModelInstanceBehavior() { Term(); }

	void Init();

	void Term();

	virtual void DrawSystematic(_MAYBE_UNUSED const Camera3D* camera);

protected:

	//=========================================================================================
	// protected variables
	//=========================================================================================

	//* member *//

	Model* model_ = nullptr;
	std::unique_ptr<DxObject::BufferResource<TransformationMatrix>> instanceBuffer_;

	//=========================================================================================
	// protected methods
	//=========================================================================================

	void CreateInstance(uint32_t instanceSize);

};