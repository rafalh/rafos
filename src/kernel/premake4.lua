project "kernel"
	kind "ConsoleApp"
	language "C"
	targetsuffix ".exe"
	files { "*.c", "*.h", "*.s", "*.S" }
	includedirs { ".", "../crt" }
	links { "crt" }
	
	linkoptions {
		"-Wl,-T,"..os.getcwd().."/link.ld", "-Wl,-Map=ntoskrnl.map",
		"-nostdlib", "-shared", "-Wl,--image-base,0x400000", "-Wl,-entry=0",
		"-Wl,--file-alignment,4096"
	}
	buildoptions { "-fomit-frame-pointer", "-ffreestanding", "-fno-exceptions" }

	local binDir = os.getcwd().."/../../bin"
	postbuildcommands { "$(PREFIX)objcopy -O binary "..binDir.."/kernel.exe "..binDir.."/kernel.bin" }
