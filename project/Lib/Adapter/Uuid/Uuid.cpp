#include "Uuid.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* windows
#include <objbase.h>

//* c++
#include <stdexcept>
#include <format>

////////////////////////////////////////////////////////////////////////////////////////////
// Uuid class methods
////////////////////////////////////////////////////////////////////////////////////////////

std::string Uuid::Serialize() const {
	return std::format("{:08x}-{:04x}-{:04x}-{:016x}", data1, data2, data3, data4);
}

Uuid Uuid::Generate() {

	GUID guid;
	if (CoCreateGuid(&guid) != S_OK) {
		throw std::runtime_error("failed to generate UUID");
	}

	Uuid uuid = {};
	uuid.data1 = guid.Data1;
	uuid.data2 = guid.Data2;
	uuid.data3 = guid.Data3;

	for (uint8_t i = 0; i < 8; ++i) {
		uuid.data4 <<= 8;
		uuid.data4 |= static_cast<uint64_t>(guid.Data4[i]);
	}

	return uuid;
}

Uuid Uuid::Deserialize(const std::string& data) {
	Uuid uuid = {};

	uuid.data1 = std::stoul(data.substr(0, 8), nullptr, 16);
	uuid.data2 = static_cast<uint16_t>(std::stoul(data.substr(9, 4), nullptr, 16));
	uuid.data3 = static_cast<uint16_t>(std::stoul(data.substr(14, 4), nullptr, 16));
	uuid.data4 = std::stoull(data.substr(19, 16), nullptr, 16);

	return uuid;
}

