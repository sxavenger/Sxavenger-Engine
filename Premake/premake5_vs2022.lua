-------------------------------------------------------------------------------------------
-- main solution
-------------------------------------------------------------------------------------------
workspace "SxavengerEngine"
	location "../project/"
	configurations { "Debug", "Develop", "Release" }
	platforms { "x64" }
	toolset "v143"

	-- スタートプロジェクトの設定
	startproject "SxavengerEngine"

	-- 出力先の設定
	targetdir "%{wks.location}/../Generated/outputs/%{cfg.buildcfg}/"         -- 出力先
	objdir    "%{wks.location}/../Generated/obj/%{prj.name}/%{cfg.buildcfg}/" -- 中間ファイル

	--- 外部projectの登録 ---
	-- DirectXTex
	externalproject "DirectXTex"
		location "../project/externals/DirectXTex"
		filename "DirectXTex_Desktop_2022_Win10"
		kind "StaticLib"
		language "C++"

	-- DirectXMesh
	externalproject "DirectXMesh"
		location "../project/externals/DirectXMesh"
		filename "DirectXMesh_Desktop_2022_Win10"
		kind "StaticLib"
		language "C++"
	
	-- imgui
	externalproject "imgui"
		location "../project/externals/imgui"
		filename "imgui"
		kind "StaticLib"
		language "C++"
	
-------------------------------------------------------------------------------------------
-- main project
-------------------------------------------------------------------------------------------
project "SxavengerEngine"
	-- projectの種類
	kind "WindowedApp"

	-- 言語
	language "c++"
	cppdialect "c++20"

	-- ファイルの追加
	files {
		"../project/*.cpp",
		"../project/*.h",

		"../project/externals/stb/Stb_include.cpp",

		"../project/Lib/**.h",
		"../project/Lib/**.cpp",
		"../project/Engine/**.h",
		"../project/Engine/**.cpp",
		"../project/Demo/**.h",
		"../project/Demo/**.cpp",
	}

	-- ファイルの除外(!xxx)
	removefiles  {
		"../project/Lib/!*/**.cpp",
		"../project/Lib/!*/**.h",
		"../project/Lib/**/!*/**.h",
		"../project/Lib/**/!*/**.cpp",
		"../project/Engine/!*/**.cpp",
		"../project/Engine/!*/**.h",
		"../project/Engine/**/!*/**.h",
		"../project/Engine/**/!*/**.cpp",
		"../project/Demo/!*/**.cpp",
		"../project/Demo/!*/**.h",
		"../project/Demo/**/!*/**.h",
		"../project/Demo/**/!*/**.cpp",
	}

	-- 追加include
	includedirs {
		"%{prj.location}",
    	"%{prj.location}/externals/nlohmann",
    	"%{prj.location}/externals/meshoptimizer",
    	"%{prj.location}/externals/imgui/imguizmo",
    	"%{prj.location}/externals/imgui",
    	"%{prj.location}/externals/DirectXTex",
    	"%{prj.location}/externals/DirectXMesh",
    	"%{prj.location}/externals/assimp/include",
    	"%{prj.location}/externals/magic_enum",
    	"%{prj.location}/externals/stb",
	}

	-- 依存プロジェクト
	dependson { "DirectXTex", "DirectXMesh", "imgui" }
	links     { "DirectXTex", "DirectXMesh", "imgui" }

	-- ビルドオプション(共通)
	warnings "High"
	buildoptions { "/utf-8" }
	flags { "MultiProcessorCompile" } -- 複数コアのでの並列コアコンパイル
	staticruntime "On"
	
	-- define定義(共通)
	defines { '_PROFILE="$(Configuration)"', "NOMINMAX" }

	-- リンカー設定(共通)
	linkoptions { "/WX" }

	-- ビルド後イベント(共通)
	postbuildcommands {
		'copy "$(WindowsSdkDir)bin\\$(TargetPlatformVersion)\\x64\\dxcompiler.dll" "$(TargetDir)dxcompiler.dll"',
  		'copy "$(WindowsSdkDir)bin\\$(TargetPlatformVersion)\\x64\\dxil.dll" "$(TargetDir)dxil.dll"'
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
		libdirs { "%{prj.location}/externals/assimp/lib/Debug" }
		links { "assimp-vc143-mtd" }
		
		-- ビルド後イベント
		postbuildcommands {
			'copy "$(SolutionDir)externals\\assimp\\lib\\Debug\\assimp-vc143-mtd.dll" "$(TargetDir)assimp-vc143-mtd.dll"'
		}

	-- Develop
	filter "configurations:Develop"
		-- ビルドオプション
		optimize "On"
		fatalwarnings { "All" }
		
		-- define定義
		defines { "NDEBUG", "_DEVELOPMENT" }

		-- リンカー設定
		libdirs { "%{prj.location}/externals/assimp/lib/Release" }
		links { "assimp-vc143-mt" }
		
		-- ビルド後イベント
		postbuildcommands {
			'copy "$(SolutionDir)externals\\assimp\\lib\\Release\\assimp-vc143-mt.dll" "$(TargetDir)assimp-vc143-mt.dll"'
		}

	-- Release
	filter "configurations:Release"
		-- ビルドオプション
		optimize "On"
		
		-- define定義
		defines { "NDEBUG" }

		-- リンカー設定
		libdirs { "%{prj.location}/externals/assimp/lib/Release" }
		links { "assimp-vc143-mt" }
		
		-- ビルド後イベント
		postbuildcommands {
			'copy "$(SolutionDir)externals\\assimp\\lib\\Release\\assimp-vc143-mt.dll" "$(TargetDir)assimp-vc143-mt.dll"'
		}
	
