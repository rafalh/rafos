project "ntoskrnl"
	kind "ConsoleApp"
	language "C"
	targetsuffix (".exe")
	objdir "../../obj/ntoskrnl"
	files { "*.c", "*.h", "*.s", "*.S" }
	includedirs { "." }
	
	linkoptions {
		"-Wl,-T,"..os.getcwd().."/link.ld", "-Wl,-Map=ntoskrnl.map",
		"-nostdlib", "-shared", "-Wl,--image-base,0x400000", "-Wl,-entry=0",
		"-Wl,--file-alignment,4096"
	}
	
--configuration "*.c"
	buildoptions { "-fomit-frame-pointer", "-ffreestanding", "-fno-exceptions" }

	local binDir = os.getcwd().."/../../bin"
	postbuildcommands { "$(PREFIX)objcopy -O binary "..binDir.."/ntoskrnl.exe "..binDir.."/ntoskrnl.bin" }
