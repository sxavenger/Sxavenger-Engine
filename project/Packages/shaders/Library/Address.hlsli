#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* library
#include "DXGIScaler.hlsli"

////////////////////////////////////////////////////////////////////////////////////////////
// Address strcture
////////////////////////////////////////////////////////////////////////////////////////////
struct Address {

	//=========================================================================================
	// public variables
	//=========================================================================================

	uint64_t address;

	//=========================================================================================
	// public merthods
	//=========================================================================================

	uint2 GetSprit() {
		uint x = (uint)((address >> 32) & 0xFFFFFFFF); //!< upper 32 bits
		uint y = (uint)(address & 0xFFFFFFFF);         //!< lower 32 bits
		return uint2(x, y);
	}

	static Address GetAddress(uint2 sprit) {
		Address address;
		address.address = ((uint64_t)sprit.x << 32) | sprit.y; //!< [x, y] = [upper 32 bits, lower 32 bits]
		return address;
	}

	static Address Null() {
		Address address;
		address.address = 0;
		return address;
	}
	
};
