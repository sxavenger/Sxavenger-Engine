#include "ExecutionContext.h"
SXAVENGER_ENGINE_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/Utility/StreamLogger.h>

//* c++
#include <ranges>

////////////////////////////////////////////////////////////////////////////////////////////
// Context class methods
////////////////////////////////////////////////////////////////////////////////////////////

void Execution::Context::Execute() {

	StreamLogger::Log(std::format("[Execution::Context]: Begin Execution Context. ptr: {:p}", reinterpret_cast<void*>(this)));

	Run(Process::Init);

	if (!conditions_.empty()) {
		while (true) {
			Run(Process::Begin);
			Run(Process::Update);
			Run(Process::Render);
			Run(Process::End);

			if (std::any_of(conditions_.begin(), conditions_.end(), [](const auto& func) { return func(); })) {
				break;
			}
		}

	} else {
		//!< loopを抜ける条件式がないので不適格.
		StreamLogger::Log("[Execution::Context] warning: loop condition is empty.");
	}

	Run(Process::Term, true);

	StreamLogger::Log(std::format("[Execution::Context]: End Execution Context. ptr: {:p}", reinterpret_cast<void*>(this)));
}

void Execution::Context::SetProcess(Process process, const std::optional<uint32_t>& layer, const ProcessFunction& function) {
	Register(process, layer.value_or(FindMinLayer(process)), function);
}

void Execution::Context::SetCondition(const ConditionFunction& func) {
	conditions_.emplace_back(func);
}

uint32_t Execution::Context::FindMinLayer(Process process) const {
	//!< FIXME: o(n)から改善

	uint32_t expected = 0;

	for (uint32_t element : processes_[static_cast<uint8_t>(process)] | std::views::keys) {

		if (element != expected) {
			return expected; // 連続していない最初の要素を返す
		}

		++expected;
	}

	return expected; // すべて連続している場合は次の要素を返す
}

void Execution::Context::Register(Process process, uint32_t layer, const ProcessFunction& function) {
	StreamLogger::AssertA(!processes_[static_cast<uint8_t>(process)].contains(layer), "state functions is layer conflict."); //!< 重複していないか確認
	processes_[static_cast<uint8_t>(process)].emplace(layer, function);
}

void Execution::Context::Run(Process process, bool isReverse) {
	if (processes_[static_cast<uint8_t>(process)].empty()) {
		return;
	}

	if (isReverse) {
		for (const auto& func : processes_[static_cast<uint8_t>(process)] | std::views::reverse | std::views::values) {
			func();
		}

	} else {
		for (auto& func : processes_[static_cast<uint8_t>(process)] | std::views::values) {
			func();
		}
	}

}
