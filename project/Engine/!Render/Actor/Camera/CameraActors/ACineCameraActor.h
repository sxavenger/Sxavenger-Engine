#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* actor
#include "../ACameraActor.h"

////////////////////////////////////////////////////////////////////////////////////////////
// ACineCameraActor class
////////////////////////////////////////////////////////////////////////////////////////////
class ACineCameraActor
	: public ACameraActor {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// CineCameraParameter structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct CineCameraParameter {

		//* member *//

		Vector2f sensor; /*_mm*/
		float focal;     /*_mm*/
		float nearZ;
		float farZ;

		//* method *//

		Matrix4x4 GetProj() const;

	};

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	ACineCameraActor()          = default;
	virtual ~ACineCameraActor() = default;

	void Init();

	void TransferProj();

	//* getter *//

	const CineCameraParameter& GetParameter() const { return parameter_; }
	CineCameraParameter& GetParameter() { return parameter_; }

protected:

	//=========================================================================================
	// protected variables
	//=========================================================================================

	CineCameraParameter parameter_;

};
