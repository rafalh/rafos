workspace "RafOS"
	location "build"
	configurations { "Debug", "Release" }
	defines { "_CRT_SECURE_NO_WARNINGS" }
	includedirs "include"
	targetdir "bin"

	buildoptions "-Wall -std=c99"
	
	configuration "Debug"
		defines { "DEBUG" }
		symbols "On"
	
	configuration "Release"
		defines { "NDEBUG" }
		optimize "Size"
	
	configuration "linux"
		defines { "LINUX" }
	
	include "src"
