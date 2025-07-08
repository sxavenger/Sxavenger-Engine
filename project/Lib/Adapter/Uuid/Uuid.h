#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* c++
#include <cstdint>
#include <string>

////////////////////////////////////////////////////////////////////////////////////////////
// Uuid class
////////////////////////////////////////////////////////////////////////////////////////////
class Uuid {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//* operator *//

	bool operator==(const Uuid& other) const {
		return data1 == other.data1 && data2 == other.data2 && data3 == other.data3 && data4 == other.data4;
	}

	bool operator!=(const Uuid& other) const {
		return !(*this == other);
	}

	//* methods *//

	std::string Serialize() const;

	//* static methods *//

	static Uuid Generate();

	static Uuid Deserialize(const std::string& data);

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	uint32_t data1; //!< first part  - 32 bits
	uint16_t data2; //!< second part - 16 bits
	uint16_t data3; //!< third part  - 16 bits
	uint64_t data4; //!< fourth part - 64 bits

	//=========================================================================================
	// friend classes
	//=========================================================================================

	friend struct std::hash<Uuid>;

};

////////////////////////////////////////////////////////////////////////////////////////////
// hash option
////////////////////////////////////////////////////////////////////////////////////////////
template <>
struct std::hash<Uuid> {
	std::size_t operator()(const Uuid& uuid) const noexcept {
		// 簡易的なハッシュ結合（Boostのhash_combine風）
		std::size_t h1 = std::hash<uint32_t>{}(uuid.data1);
		std::size_t h2 = std::hash<uint16_t>{}(uuid.data2);
		std::size_t h3 = std::hash<uint16_t>{}(uuid.data3);
		std::size_t h4 = std::hash<uint64_t>{}(uuid.data4);

		// ハッシュを統合
		std::size_t seed = h1;
		seed ^= h2 + 0x9e3779b9 + (seed << 6) + (seed >> 2);
		seed ^= h3 + 0x9e3779b9 + (seed << 6) + (seed >> 2);
		seed ^= h4 + 0x9e3779b9 + (seed << 6) + (seed >> 2);
		return seed;
	}
};
