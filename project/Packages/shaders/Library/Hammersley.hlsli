#pragma once

/* #reference
 - https://holger.dammertz.org/stuff/notes_HammersleyOnHemisphere.html
*/

////////////////////////////////////////////////////////////////////////////////////////////
// methods
////////////////////////////////////////////////////////////////////////////////////////////

uint Reversebits(uint x) {
	uint bits;
	bits = (x << 16u) | (x >> 16u);
	bits = ((x & 0x55555555u) << 1u) | ((x & 0xAAAAAAAAu) >> 1u);
	bits = ((x & 0x33333333u) << 2u) | ((x & 0xCCCCCCCCu) >> 2u);
	bits = ((x & 0x0F0F0F0Fu) << 4u) | ((x & 0xF0F0F0F0u) >> 4u);
	bits = ((x & 0x00FF00FFu) << 8u) | ((x & 0xFF00FF00u) >> 8u);
	return bits;
}

float2 Hammersley(uint i, uint N) {
	return float2(float(i) / float(N), Reversebits(i) * 2.3283064365386963e-10);
}
