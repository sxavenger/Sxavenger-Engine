//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "LightRender.hlsli"

////////////////////////////////////////////////////////////////////////////////////////////
// main
////////////////////////////////////////////////////////////////////////////////////////////
PSInput main(VSInput2d input, uint instanceId : SV_InstanceID) {

	PSInput output = (PSInput)0;
	
	output.position   = float4(input.position, 0.0f, 1.0f);
	output.instanceId = instanceId;
	
	return output;
}
