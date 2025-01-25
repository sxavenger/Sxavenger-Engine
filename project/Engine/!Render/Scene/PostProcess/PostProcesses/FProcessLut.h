#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* process
#include "../FPostProcess.h"

//* engine
#include <Engine/System/DirectX/DxObject/DxDimensionBuffer.h>

////////////////////////////////////////////////////////////////////////////////////////////
// FProcessLut class
////////////////////////////////////////////////////////////////////////////////////////////
class FProcessLut
	: public FPostProcess {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// config
	////////////////////////////////////////////////////////////////////////////////////////////

	static const uint32_t kPointCount = 4;

	static float CatmullRomInterpolation(float p0, float p1, float p2, float p3, float t);

	////////////////////////////////////////////////////////////////////////////////////////////
	// Point structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct Point {
		float input, output;
		float padding[2];
	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// Table structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct Table {

		//* member *//

		std::array<Point, kPointCount> points;

		//* methods *//

		void Init();

		void Sort();

		void SetImGuiCommand();

		float GetOutput(float input);

	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// Parameter structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct Parameter {

		//* member *//

		Table r;
		Table g;
		Table b;
	};

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	FProcessLut()          = default;
	virtual ~FProcessLut() = default;

	void Init();

	//* process *//

	void Process(const ProcessContext& context) override;

	const Parameter& GetParameter() const { return parameter_->At(0); }
	Parameter& GetParameter() { return parameter_->At(0); }

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* parameter *//

	std::unique_ptr<DxObject::DimensionBuffer<Parameter>> parameter_;


};
