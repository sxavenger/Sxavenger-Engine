#include "MonoCommon.h"
SXAVENGER_ENGINE_USING

////////////////////////////////////////////////////////////////////////////////////////////
// Common class methods
////////////////////////////////////////////////////////////////////////////////////////////

void Mono::Common::Init() {

	mono_set_dirs(
		kBinDirectory.generic_string().c_str(),
		kEtcDirectory.generic_string().c_str()
	);

#ifdef _DEVELOPMENT
	//!< Debuggerの設定
	std::string agent = std::format("--debugger-agent=transport=dt_socket,server=y,suspend={},address=127.0.0.1:55555,timeout={}", GetFlag(kSuspend), kTimeout);

	const char* options[] = {
		agent.c_str(),
		 "--soft-breakpoints",
	};

	mono_jit_parse_options(sizeof(options) / sizeof(char*), (char**)options);
	mono_debug_init(MONO_DEBUG_FORMAT_MONO);

#endif

}

void Mono::Common::Term() {
}
