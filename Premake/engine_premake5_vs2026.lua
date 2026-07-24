-------------------------------------------------------------------------------------------
-- os
-------------------------------------------------------------------------------------------
-- 先頭で作業ディレクトリをprojectに変更
os.chdir(_SCRIPT_DIR .. "/../project")

-------------------------------------------------------------------------------------------
-- main solution
-------------------------------------------------------------------------------------------
workspace "SxavengerEngine"

	-- 構成プロパティの定義
	configurations { "Debug", "Develop", "Release" }

	-- プラットフォームの設定
	platforms { "x64" }
	
	-- スタートプロジェクトの設定
	startproject "SxavengerEngine"

	-- 出力先の設定
	objdir "%{wks.location}/../Generated/obj/%{prj.name}/%{cfg.buildcfg}/" -- 中間ファイル

	filter "kind:ConsoleApp or kind:WindowedApp"
		targetdir "%{wks.location}/../Generated/outputs/%{cfg.buildcfg}/" -- 出力先(ConsoleまたはWindowsApplicationの場合)

	filter "kind:StaticLib or kind:SharedLib"
    	targetdir "%{wks.location}/../Generated/bin/%{prj.name}/%{cfg.buildcfg}/" -- 出力先(StaticLibの場合)

-------------------------------------------------------------------------------------------
-- [DirectXTex] project
-------------------------------------------------------------------------------------------
project "DirectXTex"
	-- [DirectXTex](https://github.com/microsoft/DirectXTex.git)
	-- DirectXTex_Desktop_2022_Win10.vcxprojを参照して作成.

	-- 構成プロパティの修正(DevelopをReleaseと同等に)
	removeconfigurations { "Develop" }
    configmap { ["Develop"] = "Release" }
	
	-- フォルダ指定
	location "Externals/DirectXTex"

	-- visual studioの設定
	toolset "v145"

	-- projectの種類
	kind "StaticLib"

	-- 言語
	language "c++"
	cppdialect "c++20"

	-- ファイルの追加
	files {
		"%{prj.location}/*.cpp",
		"%{prj.location}/*.h",

		-- "%{prj.location}/Shaders/**.hlsl",
		-- "%{prj.location}/Shaders/**.cmd",
	}

	-- 追加include
	includedirs {
		"%{prj.location}",
		"%{prj.location}/Shaders/Compiled",
	}

	-- ビルドオプション(共通)
	warnings "High"
	multiprocessorcompile "On" -- 複数コアのでの並列コアコンパイル
	staticruntime "On"
	floatingpoint "Fast"

	buildoptions {
		"/Zc:__cplusplus",
    	"/Zc:twoPhase-",
		"/utf-8",
	}

	-- define定義(共通)
	defines {
		"_UNICODE",
		"UNICODE",
		"WIN32",
		"_LIB",
		"_WIN32_WINNT=0x0A00",
		"_CRT_STDIO_ARBITRARY_WIDE_SPECIFIERS"
	}

	-- ShaderCompile起動
	prebuildcommands {
   		'if not exist "%{prj.location}\\Shaders\\Compiled\\BC6HEncode_EncodeBlockCS.inc" ( ' ..
        'echo Compiling shaders... && ' ..
        'cd /d "%{prj.location}\\Shaders" && ' ..
        'CompileShaders.cmd' ..
    	' )'
    }

	cleancommands {
        "del /Q %{prj.location}/Shaders/Compiled\\*.inc",
        "del /Q %{prj.location}/Shaders/Compiled\\*.pdb"
    }

	--- 構成ごとの設定 ---
	filter "configurations:Debug"
		-- ビルドオプション
			symbols "On"
			fatalwarnings { "All" }
		
	filter "configurations:Release"
		-- ビルドオプション
		optimize "On"

-------------------------------------------------------------------------------------------
-- [ImGui] project
-------------------------------------------------------------------------------------------
project "ImGui-Docking"
	-- [ImGui](https://github.com/ocornut/imgui.git)
	-- [ImGuizmo](https://github.com/CedricGuillemet/ImGuizmo.git)


	-- 構成プロパティの修正(DevelopをReleaseと同等に)
	removeconfigurations { "Develop" }
    configmap { ["Develop"] = "Release" }
	
	-- フォルダ指定
	location "Externals/imgui"

	-- visual studioの設定
	toolset "v145"

	-- projectの種類
	kind "StaticLib"

	-- 言語
	language "c++"
	cppdialect "c++20"

	-- ファイルの追加
	files {
		"%{prj.location}/*.cpp",
		"%{prj.location}/*.h",
		
		-- Win32, DirectX12を使用
		"%{prj.location}/Backends/imgui_impl_dx12.cpp",
		"%{prj.location}/Backends/imgui_impl_dx12.h",
		"%{prj.location}/Backends/imgui_impl_win32.cpp",
		"%{prj.location}/Backends/imgui_impl_win32.h",
	}

	-- 追加include
	includedirs {
		"%{prj.location}",
		"%{prj.location}/Backends",
	}

	-- ビルドオプション(共通)
	warnings "High"
	multiprocessorcompile "On" -- 複数コアのでの並列コアコンパイル
	staticruntime "On"
	buildoptions { "/utf-8" }

	--- 構成ごとの設定 ---
	filter "configurations:Debug"
		-- ビルドオプション
			symbols "On"
			fatalwarnings { "All" }
		
	filter "configurations:Release"
		-- ビルドオプション
		optimize "On"

-------------------------------------------------------------------------------------------
-- [meshoptimizer] project
-------------------------------------------------------------------------------------------
project "meshoptimizer"
	-- [meshoptimizer](https://github.com/zeux/meshoptimizer.git)

	-- 構成プロパティの修正(DevelopをReleaseと同等に)
	removeconfigurations { "Develop" }
    configmap { ["Develop"] = "Release" }
	
	-- フォルダ指定
	location "Externals/meshoptimizer"

	-- visual studioの設定
	toolset "v145"

	-- projectの種類
	kind "StaticLib"

	-- 言語
	language "c++"
	cppdialect "c++20"

	-- ファイルの追加
	files {
		"%{prj.location}/**.cpp",
		"%{prj.location}/**.h",
	}

	-- 追加include
	includedirs {
		"%{prj.location}",
	}

	-- ビルドオプション(共通)
	warnings "High"
	multiprocessorcompile "On" -- 複数コアのでの並列コアコンパイル
	staticruntime "On"
	buildoptions { "/utf-8" }

	--- 構成ごとの設定 ---
	filter "configurations:Debug"
		-- ビルドオプション
			symbols "On"
			fatalwarnings { "All" }
		
	filter "configurations:Release"
		-- ビルドオプション
		optimize "On"

-------------------------------------------------------------------------------------------
-- Script c# project
-------------------------------------------------------------------------------------------
project "Script"

	-- フォルダ指定
	location "Assets/script"

	-- projectの種類
	kind "SharedLib"

	-- 言語
    language "C#"
	architecture "x64"

	-- 使用する .NET バージョン
    dotnetframework "net4.8"

    files {
        "%{prj.location}/**.cs"
    }

    filter "configurations:Debug"
        optimize "Off"
		
	filter "configurations:Develop"
        optimize "Off"

    filter "configurations:Release"
        optimize "On"

-------------------------------------------------------------------------------------------
-- main c++ project
-------------------------------------------------------------------------------------------
project "SxavengerEngine"

	-- visual studioの設定 --
	toolset "v145"

	-- projectの種類 --
	kind "WindowedApp"

	-- 言語 --
	language "c++"
	cppdialect "c++20"

	-- ファイルの追加 --
	files {
		"%{prj.location}/*.cpp",
		"%{prj.location}/*.h",
		
		-- TODO: 専用のsolutionを作成する
		"%{prj.location}/Externals/stb/Stb_include.cpp",

		"%{prj.location}/Lib/**.h",
		"%{prj.location}/Lib/**.cpp",
		"%{prj.location}/Engine/**.h",
		"%{prj.location}/Engine/**.cpp",
	}

	-- ファイルの除外(!xxx) --
	removefiles  {
		"Lib/!*/**.cpp",
		"Lib/!*/**.h",
		"Lib/**/!*/**.h",
		"Lib/**/!*/**.cpp",
		"Engine/!*/**.cpp",
		"Engine/!*/**.h",
		"Engine/**/!*/**.h",
		"Engine/**/!*/**.cpp",
		"Demo/!*/**.cpp",
		"Demo/!*/**.h",
		"Demo/**/!*/**.h",
		"Demo/**/!*/**.cpp",
	}

	-- 追加include --
	includedirs {
		"%{prj.location}",
    	"%{prj.location}/Externals/nlohmann", -- [nlohmann json](https://github.com/nlohmann/json.git)
    	"%{prj.location}/Externals/meshoptimizer", -- [meshoptimizer](https://github.com/zeux/meshoptimizer.git)
    	"%{prj.location}/Externals/imgui", -- [ImGui](https://github.com/ocornut/imgui.git)
		"%{prj.location}/Externals/imgui/backends",
    	"%{prj.location}/Externals/DirectXTex", -- [DirectXTex](https://github.com/microsoft/DirectXTex.git)
    	"%{prj.location}/Externals/assimp/include", -- [assimp](https://github.com/assimp/assimp.git)
    	"%{prj.location}/Externals/magic_enum", -- [magic_enum](https://github.com/Neargye/magic_enum.git)
    	"%{prj.location}/Externals/stb", -- [stb](https://github.com/nothings/stb.git)
		"%{prj.location}/Externals/mono/include", -- [Mono](https://www.mono-project.com/)
		"%{prj.location}/Externals/PixEvents/include", -- [PixEvents](https://github.com/microsoft/PixEvents.git)
	}

	-- 依存プロジェクト --
	dependson {
		"DirectXTex",
		"ImGui-Docking",
		"meshoptimizer",
		"Script"
	}

	links {
		"DirectXTex",
		"ImGui-Docking",
		"meshoptimizer",
	}

	-- ビルドオプション(共通) --
	warnings "High"
	multiprocessorcompile "On" -- 複数コアのでの並列コアコンパイル
	staticruntime "On"
	buildoptions { "/utf-8" }
	
	-- define定義(共通) --
	defines { '_PROFILE="$(Configuration)"', "NOMINMAX" }

	-- リンカー設定(共通) --
	linkoptions {
		"/WX",
		"/IGNORE:4099", -- [LNK4099](https://learn.microsoft.com/ja-jp/cpp/error-messages/tool-errors/linker-tools-warning-lnk4099)
	}

	-- ビルド後イベント --
	postbuildcommands {
		-- dxcompiler関係
		'copy "$(WindowsSdkDir)bin\\$(TargetPlatformVersion)\\x64\\dxcompiler.dll" "$(TargetDir)dxcompiler.dll"',
  		'copy "$(WindowsSdkDir)bin\\$(TargetPlatformVersion)\\x64\\dxil.dll" "$(TargetDir)dxil.dll"',
	}

	--- 外部プログラムごとの設定 ---
	--- Mono
	-- リンカー設定 --
	libdirs {
		"%{prj.location}/Externals/mono/lib"
	}
	
	-- 依存ファイル --
	links {
		"mono-2.0-sgen"
	}

	-- ビルド後イベント --
	postbuildcommands {
		'copy "Externals\\mono\\bin\\mono-2.0-sgen.dll" "$(TargetDir)mono-2.0-sgen.dll"',
	}

	-- Assimp
	filter "configurations:Debug"
		-- リンカー設定 --
		libdirs {
			"%{prj.location}/Externals/assimp/lib/Debug"
		}

		-- 依存ファイル --
		links {
			"assimp-vc145-mtd",
			"zlibstaticd"
		}

	filter "configurations:Develop or configurations:Release"
		-- リンカー設定 --
		libdirs {
			"%{prj.location}/Externals/assimp/lib/Release"
		}

		-- 依存ファイル --
		links {
			"assimp-vc145-mt",
			"zlibstatic"
		}
	
	-- PixEvents
	filter "configurations:Debug"
		-- リンカー設定 --
		libdirs {
			"%{prj.location}/Externals/PixEvents/lib/Debug"
		}

		-- 依存ファイル --
		links {
			"WinPixEventRuntime",
		}

		-- ビルド後イベント --
		postbuildcommands {
			'copy "Externals\\PixEvents\\bin\\Debug\\WinPixEventRuntime.dll" "$(TargetDir)WinPixEventRuntime.dll"'
		}
	
	filter "configurations:Develop"
		-- リンカー設定 --
		libdirs {
			"%{prj.location}/Externals/PixEvents/lib/Release"
		}

		-- 依存ファイル --
		links {
			"WinPixEventRuntime",
		}

		-- ビルド後イベント --
		postbuildcommands {
			'copy "Externals\\PixEvents\\bin\\Release\\WinPixEventRuntime.dll" "$(TargetDir)WinPixEventRuntime.dll"'
		}
	
	--- project構成ごとのビルドオプション設定 ---
	-- Debug
	filter "configurations:Debug"
		-- ビルドオプション
		symbols "On"
		
		-- define定義
		defines { "_DEVELOPMENT" }

	-- Develop
	filter "configurations:Develop"
		-- ビルドオプション
		optimize "On"
		
		-- define定義
		defines { "_DEVELOPMENT" }

	-- Release
	filter "configurations:Release"
		-- ビルドオプション
		optimize "On"

		buildoptions {
			"/wd4100" -- [C4100](https://learn.microsoft.com/ja-jp/cpp/error-messages/compiler-warnings/compiler-warning-level-4-c4100)
		}