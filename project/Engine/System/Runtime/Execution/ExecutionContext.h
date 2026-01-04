#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/Foundation.h>

//* external
#include <magic_enum.hpp>

//* c++
#include <cstdint>
#include <functional>
#include <optional>
#include <array>
#include <map>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////////////
// Execution namespace
////////////////////////////////////////////////////////////////////////////////////////////
namespace Execution {

	////////////////////////////////////////////////////////////////////////////////////////////
	// Process enum class
	////////////////////////////////////////////////////////////////////////////////////////////
	enum class Process : uint8_t { //!< 実行順

		//* Init
		Init,

		//* Loop
		Begin,
		Update,
		Render,
		End,

		//* Term
		Term, //!< reverse process order.

	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// Context class
	////////////////////////////////////////////////////////////////////////////////////////////
	//! @brief ExecutionのContextクラス.
	class Context {
	public:

		////////////////////////////////////////////////////////////////////////////////////////////
		// using
		////////////////////////////////////////////////////////////////////////////////////////////

		using ProcessFunction   = std::function<void()>;
		using ConditionFunction = std::function<bool()>;

	public:

		//=========================================================================================
		// public method
		//=========================================================================================

		//! @brief contextの実行処理
		void Execute();

		//! @brief GameLoopの処理を登録する
		//! @param[in] process  実行する箇所
		//! @param[in] layer    処理順序
		//! @param[in] function 処理内容
		void SetProcess(Process process, const std::optional<uint32_t>& layer, const ProcessFunction& function);

		//! @brief GameLoopの終了条件を登録する
		//! @param[in] func 終了条件
		void SetCondition(const ConditionFunction& func);

	private:

		//=========================================================================================
		// private variables
		//=========================================================================================

		std::array<std::map<uint32_t, ProcessFunction>, magic_enum::enum_count<Process>()> processes_;
		std::list<ConditionFunction>                                                       conditions_;

		//=========================================================================================
		// private methods
		//=========================================================================================

		//* helper methods *//

		uint32_t FindMinLayer(Process process) const;

		void Register(Process process, uint32_t layer, const ProcessFunction& function);

		void Run(Process process, bool isReverse = false);

	};

}

SXAVENGER_ENGINE_NAMESPACE_END
