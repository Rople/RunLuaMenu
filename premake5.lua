solution "RunLuaMenu"
	configurations { "Release" }
	platforms { "x32" }
	
	language "C++"
	characterset	"MBCS"
	location ( "project/" .. os.get() .."-".. _ACTION )
	targetdir ( "target/" .. os.get() .. "/" )

	filter "platforms:x32"
		architecture "x32"

	project "RunLuaMenu"
		kind	"SharedLib"
		targetname "RunLuaMenu"
		flags { "Symbols", "NoEditAndContinue", "NoPCH", "StaticRuntime", "EnableSSE" }
		links "bootil_static"
		includedirs { "include", "bootil/include" }

        if os.is( "linux" ) then
                buildoptions { "-fPIC", "-pthread" }
                linkoptions { "-pthread" }
        end

		files {
			"src/main.cpp"
		}

	include "bootil/projects/bootil_premake5.lua"