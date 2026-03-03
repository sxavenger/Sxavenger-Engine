#pragma once

#ifndef RAYTRACING_LIBRARY
#define RAYTRACING_LIBRARY

////////////////////////////////////////////////////////////////////////////////////////////
// define
////////////////////////////////////////////////////////////////////////////////////////////

//* payload
#define PAYLOAD [raypayload]
#define READ_QUALIFIER(...) read(__VA_ARGS__)
#define WRITE_QUALIFIER(...) write(__VA_ARGS__)
#define QUALIFIRE_DEFAULT READ_QUALIFIER(caller) : WRITE_QUALIFIER(closesthit, miss)

//* shaders
#define RAYGENERATION [shader("raygeneration")]
#define ANYHIT        [shader("anyhit")]
#define CLOSESTHIT    [shader("closesthit")]
#define MISS          [shader("miss")]

////////////////////////////////////////////////////////////////////////////////////////////
// constant variables
////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////
// Attribtue structure
////////////////////////////////////////////////////////////////////////////////////////////
struct Attribute {

	//=========================================================================================
	// public variables
	//=========================================================================================
	
	float2 barys;

	//=========================================================================================
	// public methods
	//=========================================================================================

	float3 GetBarycentrics() {
		return float3(
			1.0f - barys.x - barys.y,
			barys.x,
			barys.y
		);
	}
	
};

#endif
