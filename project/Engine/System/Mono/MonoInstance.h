#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* mono
#include "MonoCommon.h"
#include "MonoDomain.h"
#include "MonoAssembly.h"

//* c++
#include <string>
#include <optional>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////////////
// Mono namespace
////////////////////////////////////////////////////////////////////////////////////////////
namespace Mono {

	////////////////////////////////////////////////////////////////////////////////////////////
	// Instance class
	////////////////////////////////////////////////////////////////////////////////////////////
	//! @brief C#(Mono)側のオブジェクトインスタンスを保持し, メソッド呼び出しを行うラッパー
	class Instance {
	public:

		////////////////////////////////////////////////////////////////////////////////////////////
		// TypeInfo structure
		////////////////////////////////////////////////////////////////////////////////////////////
		//! @brief 対象クラスの名前空間とクラス名を保持する構造体
		struct TypeInfo {
		public:

			//=========================================================================================
			// public methods
			//=========================================================================================

			std::string GetName() const { return namespace_ + "." + class_; }

			//=========================================================================================
			// public variables
			//=========================================================================================

			std::string namespace_;
			std::string class_;
		};

	public:

		//=========================================================================================
		// public methods
		//=========================================================================================

		Instance() = default;

		void Create(
			const Mono::Domain* domain, const Mono::Assembly* assembly,
			const std::string& _namespace, const std::string& _class
		);

		void Reset();

		void CallFunction(const std::string& name);

		//* operator [assign] *//

		Mono::Instance& operator=(std::nullptr_t);

		//* operator [equal] *//

		bool operator==(std::nullptr_t) const { return instance_ == nullptr; }
		bool operator!=(std::nullptr_t) const { return instance_ != nullptr; }

		//* getter *//

	private:

		//=========================================================================================
		// private variables
		//=========================================================================================

		TypeInfo info_;

		MonoClass* class_     = nullptr;
		MonoObject* instance_ = nullptr;

		std::optional<uint32_t> handle_ = std::nullopt;

	};

}

SXAVENGER_ENGINE_NAMESPACE_END
