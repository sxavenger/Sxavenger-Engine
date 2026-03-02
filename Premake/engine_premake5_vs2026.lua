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
	targetdir "%{wks.location}/../generated/outputs/%{cfg.buildcfg}/"         -- 出力先
	objdir    "%{wks.location}/../generated/obj/%{prj.name}/%{cfg.buildcfg}/" -- 中間ファイル

	--- 外部projectの登録 ---
	-- DirectXTex
	externalproject "DirectXTex"
		location "externals/DirectXTex"
		filename "DirectXTex_Desktop_2022_Win10"
		kind "StaticLib"
		language "C++"

		configmap {
        	["Develop"] = "Release",
 		}
	
	-- imgui
	externalproject "imgui"
		location "externals/imgui"
		filename "imgui"
		kind "StaticLib"
		language "C++"
		
		configmap {
        	["Develop"] = "Release",
 		}

-------------------------------------------------------------------------------------------
-- main c++ project
-------------------------------------------------------------------------------------------
project "SxavengerEngine"

	-- visual studioの設定
	toolset "v143"

	-- projectの種類
	kind "WindowedApp"

	-- 言語
	language "c++"
	cppdialect "c++20"

	-- ファイルの追加
	files {
		"%{prj.location}/*.cpp",
		"%{prj.location}/*.h",
		
		-- TODO: 専用のsolutionを作成する
		"%{prj.location}/Externals/stb/Stb_include.cpp",
		"%{prj.location}/Externals/meshoptimizer/*.h",
		"%{prj.location}/Externals/meshoptimizer/*.cpp",
		"%{prj.location}/Externals/mono/include/**.cpp",
		"%{prj.location}/Externals/mono/include/**.h",

		"%{prj.location}/Lib/**.h",
		"%{prj.location}/Lib/**.cpp",
		"%{prj.location}/Engine/**.h",
		"%{prj.location}/Engine/**.cpp",
		"%{prj.location}/Demo/**.h",
		"%{prj.location}/Demo/**.cpp",
	}

	-- ファイルの除外(!xxx)
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

	-- 追加include
	includedirs {
		"%{prj.location}",
    	"%{prj.location}/Externals/nlohmann", -- [nlohmann json](https://github.com/nlohmann/json.git)
    	"%{prj.location}/Externals/meshoptimizer", -- [meshoptimizer](https://github.com/zeux/meshoptimizer.git)
    	"%{prj.location}/Externals/imgui", -- [ImGui](https://github.com/ocornut/imgui.git)
    	"%{prj.location}/Externals/imgui/imguizmo", -- [ImGuizmo](https://github.com/CedricGuillemet/ImGuizmo.git)
    	"%{prj.location}/Externals/DirectXTex", -- [DirectXTex](https://github.com/microsoft/DirectXTex.git)
    	"%{prj.location}/Externals/assimp/include", -- [assimp](https://github.com/assimp/assimp.git)
    	"%{prj.location}/Externals/magic_enum", -- [magic_enum](https://github.com/Neargye/magic_enum.git)
    	"%{prj.location}/Externals/stb", -- [stb](https://github.com/nothings/stb.git)
		"%{prj.location}/Externals/mono/include", -- [Mono](https://www.mono-project.com/)
	}

	-- 依存プロジェクト
	dependson { "DirectXTex", "imgui", "Script" }
	links     { "DirectXTex", "imgui" }

	-- ビルドオプション(共通)
	warnings "High"
	buildoptions { "/utf-8" }
	multiprocessorcompile "On" -- 複数コアのでの並列コアコンパイル
	staticruntime "On"
	
	-- define定義(共通)
	defines { '_PROFILE="$(Configuration)"', "NOMINMAX" }

	-- 警告の抑制
	disablewarnings { "4324" }

	-- リンカー設定(共通)
	linkoptions {
		"/WX",
		"/IGNORE:4099",
		"/IGNORE:4099",
	}

	-- リンカー設定(共通)
		libdirs {
			"%{prj.location}/Externals/mono/lib"
		}

	-- 依存ファイル(共通)
	links {
		"mono-2.0-sgen.lib"
	}

	-- ビルド後イベント(共通)
	postbuildcommands {
		-- dxcompiler関係
		'copy "$(WindowsSdkDir)bin\\$(TargetPlatformVersion)\\x64\\dxcompiler.dll" "$(TargetDir)dxcompiler.dll"',
  		'copy "$(WindowsSdkDir)bin\\$(TargetPlatformVersion)\\x64\\dxil.dll" "$(TargetDir)dxil.dll"',

		-- mono関係
		'copy "Externals\\mono\\bin\\mono-2.0-sgen.dll" "$(TargetDir)mono-2.0-sgen.dll"',
	}

	--- 構成ごとの設定 ---
	-- Debug
	filter "configurations:Debug"
		-- ビルドオプション
		symbols "On"
		fatalwarnings { "All" }
		
		-- define定義
		defines { "DEBUG", "_DEVELOPMENT" }

		-- リンカー設定
		libdirs {
			"%{prj.location}/Externals/assimp/lib/Debug"
		}

		-- 依存ファイル
		links {
			"assimp-vc145-mtd",
			"zlibstaticd"
		}

	-- Develop
	filter "configurations:Develop"
		-- ビルドオプション
		optimize "On"
		fatalwarnings { "All" }
		
		-- define定義
		defines { "NDEBUG", "_DEVELOPMENT" }

		-- リンカー設定
		libdirs {
			"%{prj.location}/Externals/assimp/lib/Release"
		}

		-- 依存ファイル
		links {
			"assimp-vc145-mt",
			"zlibstatic"
		}

	-- Release
	filter "configurations:Release"
		-- ビルドオプション
		optimize "On"
		
		-- define定義
		defines { "NDEBUG" }

		-- リンカー設定
		libdirs {
			"%{prj.location}/Externals/assimp/lib/Release",
		}

		-- 依存ファイル
		links {
			"assimp-vc145-mt",
			"zlibstatic"
		}

-------------------------------------------------------------------------------------------
-- script c# project
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