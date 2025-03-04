#include "GameLoop.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/Utility/Logger.h>

//* c++
#include <algorithm>
#include <format>
#include <ranges>

////////////////////////////////////////////////////////////////////////////////////////////
// Context class methods
////////////////////////////////////////////////////////////////////////////////////////////

void GameLoop::Context::Run() {

	Log(std::format("[GameLoop::Context]: Begin Loop. ptr: {:p}", reinterpret_cast<void*>(this)));
	Loop();
	Log(std::format("[GameLoop::Context]: End Loop. ptr: {:p}", reinterpret_cast<void*>(this)));

	Clear();
}

void GameLoop::Context::SetProcess(Process process, const std::optional<uint32_t>& layer, const ProcessFunction& function) {
	RegisterProcess(process, layer.value_or(FindMinLayer(process)), function);
}

uint32_t GameLoop::Context::FindMinLayer(Process process) const {
	uint32_t expected = 0;

	for (const auto& element : processes_[static_cast<uint8_t>(process)]) {

		if (element.first != expected) {
			return expected; // 連続していない最初の要素を返す
		}

		++expected;
	}

	return expected; // すべて連続している場合は次の要素を返す
}

void GameLoop::Context::RegisterProcess(Process process, uint32_t layer, const ProcessFunction& function) {
	Assert(!processes_[static_cast<uint8_t>(process)].contains(layer), "state functions is layer conflict."); //!< 重複していないか確認
	processes_[static_cast<uint8_t>(process)].emplace(layer, function);
}

void GameLoop::Context::Execute(Process process, bool isReverse) {
	if (processes_[static_cast<uint8_t>(process)].empty()) {
		return;
	}

	if (isReverse) {
		for (const auto& [layer, func] : processes_[static_cast<uint8_t>(process)] | std::views::reverse) {
			func();
		}

	} else {
		for (const auto& [layer, func] : processes_[static_cast<uint8_t>(process)] | std::views::common) {
			func();
		}
	}
}

void GameLoop::Context::Loop() {
	Execute(Process::Init);

	if (!conditions_.empty()) {
		while (true) {
			Execute(Process::Begin);
			Execute(Process::Update);
			Execute(Process::Render);
			Execute(Process::End);

			if (std::any_of(conditions_.begin(), conditions_.end(), [](const auto& func) { return func(); })) {
				break;
			}
		}

	} else {
		Log("[GameLoop::Context] warning: while condition is empty.");
		//!< whileを抜ける条件式がないので不適格.
	}

	Execute(Process::Term, true);
}

void GameLoop::Context::Clear() {
	for (uint8_t i = 0; i < kProcessCount; ++i) {
		processes_[i].clear();
	}

	conditions_.clear();
}

////////////////////////////////////////////////////////////////////////////////////////////
// Collection class methods
////////////////////////////////////////////////////////////////////////////////////////////

void GameLoop::Collection::Run() {
	Context::Run();

	while (!interfaces_.empty()) {
		interfaces_.back()->Term();
		interfaces_.pop_back();
	}
}
