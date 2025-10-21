#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/Runtime/Framework/IFramework.h>

//* c++
#include <cstdint>
#include <functional>
#include <array>
#include <optional>
#include <map>
#include <list>
#include <concepts>
#include <deque>
#include <memory>

////////////////////////////////////////////////////////////////////////////////////////////
// GameLoop namespace
////////////////////////////////////////////////////////////////////////////////////////////
//! @brief Applicationの実行ループを管理する名前空間.
namespace GameLoop {

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
	constexpr uint8_t kProcessCount = static_cast<uint8_t>(Process::Term) + 1;

	////////////////////////////////////////////////////////////////////////////////////////////
	// Process Function using
	////////////////////////////////////////////////////////////////////////////////////////////

	using ProcessFunction   = std::function<void()>;
	using ConditionFunction = std::function<bool()>; //!< true: break loop

	////////////////////////////////////////////////////////////////////////////////////////////
	// State Array using
	////////////////////////////////////////////////////////////////////////////////////////////
	template <class T>
	using ProcessArray = std::array<T, kProcessCount>;

	////////////////////////////////////////////////////////////////////////////////////////////
	// Context class
	////////////////////////////////////////////////////////////////////////////////////////////
	class Context
		: public IFramework {
	public:

		//=========================================================================================
		// public method
		//=========================================================================================

		Context()                   = default;
		virtual ~Context() override = default;

		virtual void Run() override;

		void SetProcess(Process process, const std::optional<uint32_t>& layer, const ProcessFunction& function);

		void SetCondition(const ConditionFunction& func) { conditions_.emplace_back(func); }

	private:

		//=========================================================================================
		// private variables
		//=========================================================================================

		ProcessArray<std::map<uint32_t, ProcessFunction>> processes_;
		std::list<ConditionFunction>                      conditions_;

		//=========================================================================================
		// private method
		//=========================================================================================

		uint32_t FindMinLayer(Process process) const;

		void RegisterProcess(Process process, uint32_t layer, const ProcessFunction& function);

		void Execute(Process process, bool isReverse = false);

		void Loop();

		void Clear();

	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// Interface class
	////////////////////////////////////////////////////////////////////////////////////////////
	class Interface {
	public:

		//=========================================================================================
		// public method
		//=========================================================================================

		virtual ~Interface() = default;

		virtual void Init(Context* context) = 0;

		virtual void Term() = 0;

	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// Interface concept
	////////////////////////////////////////////////////////////////////////////////////////////
	template <class T>
	concept InterfaceConcept = std::derived_from<T, Interface>;

	////////////////////////////////////////////////////////////////////////////////////////////
	// Collection class
	////////////////////////////////////////////////////////////////////////////////////////////
	class Collection
		: public Context {
	public:

		//=========================================================================================
		// public method
		//=========================================================================================

		template <InterfaceConcept T>
		void Push();

		void Run() override;

	private:

		//=========================================================================================
		// private variables
		//=========================================================================================

		std::deque<std::shared_ptr<Interface>> interfaces_;
		// todo: vector<unique_ptr<>>に変更

	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// Collection class template method
	////////////////////////////////////////////////////////////////////////////////////////////

	template <InterfaceConcept T>
	inline void Collection::Push() {
		auto& ptr = interfaces_.emplace_back(std::make_shared<T>());
		ptr->Init(this);
	}

}
