solution "CubeOS"
	configurations { "Debug", "Release" }
	defines { "_CRT_SECURE_NO_WARNINGS" }
	includedirs "include"
	
	configuration "Debug"
		defines { "DEBUG" }
		flags { "Symbols" }
	
	configuration "Release"
		defines { "NDEBUG" }
		flags { "OptimizeSize" }
	
	configuration "linux"
		defines { "LINUX" }
	
	include "src"
