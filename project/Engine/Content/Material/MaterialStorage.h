#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* material
#include "Material.h"

////////////////////////////////////////////////////////////////////////////////////////////
// MaterialStorage class
////////////////////////////////////////////////////////////////////////////////////////////
class MaterialStorage {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// using
	////////////////////////////////////////////////////////////////////////////////////////////
	using Storage = std::unordered_map<std::string, std::unique_ptr<Material>>;

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//* storage option *//

	Material* GetMaterial(const std::string& name);

	void Clear() { storage_.clear(); }

	//* json *//

	void OutputJson() const;

	void InputJson();

	//* getter *//

	const Storage& GetStorage() const { return storage_; }

	//* singleton *//

	static MaterialStorage* GetInstance();

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	Storage storage_;

	//* json parameter *//

	static const std::filesystem::path kJsonDirectory;

};

////////////////////////////////////////////////////////////////////////////////////////////
// singleton
////////////////////////////////////////////////////////////////////////////////////////////
static MaterialStorage* sMaterialStorage = MaterialStorage::GetInstance();
