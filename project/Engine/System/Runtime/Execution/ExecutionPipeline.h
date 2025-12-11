#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* execution
#include "ExecutionInterface.h"

//* engine
#include <Engine/Foundation.h>
#include <Engine/System/Runtime/Framework/FrameworkInterface.h>

//* c++
#include <concepts>
#include <list>
#include <memory>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////////////
// Execution namespace
////////////////////////////////////////////////////////////////////////////////////////////
namespace Execution {

	////////////////////////////////////////////////////////////////////////////////////////////
	// Pipeline class
	////////////////////////////////////////////////////////////////////////////////////////////
	class Pipeline
		: public Context, public Framework::Interface {
	public:

		//=========================================================================================
		// public method
		//=========================================================================================

		//! @brief 処理Interfaceを追加する
		template <InterfaceConcept T>
		void Push();

		//! @brief 実行
		void Run() override;

	private:

		//=========================================================================================
		// private variables
		//=========================================================================================

		std::list<std::unique_ptr<Execution::Interface>> interfaces_;

	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// Pipeline class template methods
	////////////////////////////////////////////////////////////////////////////////////////////

	template <InterfaceConcept T>
	void Pipeline::Push() {
		auto& ptr = interfaces_.emplace_back(std::make_shared<T>());
		ptr->Init(this);
	}

}

SXAVENGER_ENGINE_NAMESPACE_END
