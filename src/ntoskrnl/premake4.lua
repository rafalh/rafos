project "ntoskrnl"
	kind "ConsoleApp"
	language "C"
	files { "**.h", "**.c", "**.s", "**.S" }
	includedirs { ".", ".." }
	
	linkoptions { "-nostdlib", "-shared", "-Wl,--image-base,0x400000", "-Wl,-T,link.ld", "-Wl,-Map-ntoskrnl.map" }
	
--configuration "*.c"
	buildoptions { "-fomit-frame-pointer", "-fno-builtin", "-nostdinc" }
	
