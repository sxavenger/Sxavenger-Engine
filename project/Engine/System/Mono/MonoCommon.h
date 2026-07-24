#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/Foundation.h>

//* externals
#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>
#include <mono/metadata/object.h>
#include <mono/metadata/appdomain.h>
#include <mono/metadata/debug-helpers.h>
#include <mono/metadata/exception.h>
#include <mono/metadata/mono-debug.h>

//* c++
#include <filesystem>
#include <string>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////////////
// Mono namespace
////////////////////////////////////////////////////////////////////////////////////////////
namespace Mono {

	////////////////////////////////////////////////////////////////////////////////////////////
	// Common class
	////////////////////////////////////////////////////////////////////////////////////////////
	class Common {
	public:

		//=========================================================================================
		// public methods
		//=========================================================================================

		static void Init();

		static void Term();

	private:

		//=========================================================================================
		// private variables
		//=========================================================================================

		static inline const std::filesystem::path kDirectory = L"./Packages/mono";
		static inline const std::filesystem::path kBinDirectory = kDirectory / L"bin";
		static inline const std::filesystem::path kEtcDirectory = kDirectory / L"etc";

		static const bool kSuspend = false; //!< デバッガー接続まで待機するかどうか
		static const int  kTimeout = 0;     //!< デバッガー接続待ちのタイムアウト(ms)

		// FIXME: suspend=trueでないとDebuggerが接続できない.

		//=========================================================================================
		// private methods
		//=========================================================================================

		static std::string GetFlag(bool flag) { return flag ? "y" : "n"; }

	};

}

SXAVENGER_ENGINE_NAMESPACE_END

