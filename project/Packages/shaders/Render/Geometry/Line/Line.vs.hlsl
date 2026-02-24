//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "LineGeometry.hlsli"

//=========================================================================================
// buffers
//=========================================================================================

////////////////////////////////////////////////////////////////////////////////////////////
// main
////////////////////////////////////////////////////////////////////////////////////////////
GeometryPSInput main(MeshVertex input, uint instanceId : SV_InstanceID) {

	GeometryPSInput output = (GeometryPSInput)0;
	
	output.position = mul(gTransform.Transform(input.position), kViewProj);
	
	output.instanceId = instanceId;
	output.clip       = output.position.z; //!< near分のclip
	
	return output;
}
