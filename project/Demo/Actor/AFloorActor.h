#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* actor
#include <Engine/!Render/Actor/Geometry/GeometryActors/AManualGeometryActor.h>

//* lib
#include <Lib/Geometry/Vector2.h>

////////////////////////////////////////////////////////////////////////////////////////////
// AFloorActor class
////////////////////////////////////////////////////////////////////////////////////////////
class AFloorActor
	: public AManualGeometryActor {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	AFloorActor()  = default;
	~AFloorActor() = default;

	void Init();

	void Term();

	void SetSize(const Vector2f& size);

	void SetTexture(const AssetObserver<AssetTexture>& texture) { texture_ = texture; }

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	Vector2f size_ = { 1.0f, 1.0f };

	//=========================================================================================
	// private methods
	//=========================================================================================

	void UpdateInputAssembler();
	void UpdateTextureComponent();

};
