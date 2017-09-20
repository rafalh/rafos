workspace "RafOS"
	location "build"
	configurations { "Debug", "Release" }
	defines { "_CRT_SECURE_NO_WARNINGS" }
	includedirs "include"
	targetdir "bin"
	objdir "obj"
	buildoptions {
		-- FIXME
		"-Wno-int-to-pointer-cast",
		"-Wno-pointer-to-int-cast",
		"-Wno-implicit-function-declaration",
	}
	
	configuration "Debug"
		defines { "DEBUG" }
		symbols "On"
	
	configuration "Release"
		defines { "NDEBUG" }
		optimize "Size"
	
	configuration "linux"
		defines { "LINUX" }
	
	include "src"
