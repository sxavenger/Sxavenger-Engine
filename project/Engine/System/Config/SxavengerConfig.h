#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* c++
#include <string>
#include <filesystem>

////////////////////////////////////////////////////////////////////////////////////////////
// Version
////////////////////////////////////////////////////////////////////////////////////////////

static const std::string kSxavengerEngineVersion = "3.2";

////////////////////////////////////////////////////////////////////////////////////////////
// Directory
////////////////////////////////////////////////////////////////////////////////////////////

static const std::filesystem::path kPackagesDirectory       = "packages";
static const std::filesystem::path kPackagesShaderDirectory = kPackagesDirectory / "shaders";

static const std::filesystem::path kAssetsDirectory         = "assets";
static const std::filesystem::path kAssetsJsonDirectory     = kAssetsDirectory / "json";
static const std::filesystem::path kAssetsModelDirectory    = kAssetsDirectory / "models";
