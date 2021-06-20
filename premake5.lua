workspace "Quadtree"
	configurations { "Debug", "Release" }

project "Quadtree"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"

	targetdir "Bin/%{cfg.buildcfg}"
	objdir "Obj"

	files { "Source/**.hpp", "Source/**.cpp" }
	links { "sfml-system", "sfml-window", "sfml-graphics" }

	filter { "configurations:Debug" }
		symbols "On"
		defines { "DEBUG" }

	filter { "configurations:Release" }
		optimize "On"