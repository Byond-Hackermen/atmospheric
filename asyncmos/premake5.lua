assert(_ACTION ~= nil, "no action (vs20**, gmake or xcode for example) provided!")

-- Clean Function --
newaction {
   trigger     = "clean",
   description = "cleans the repo",
   execute     = function()
      print("Cleaning project")
      os.rmdir("./asyncmos")
   end
}

if _ACTION ~= "vs2017" and _ACTION ~= "clean" then
	error("The only supported compilation platform for this project (asyncmos) on Windows is Visual Studio 2017.")
end


workspace "asyncmos"
	language "C++"
	cppdialect "C++17"
	systemversion "latest"
	
	kind "SharedLib"
	targetextension ".dll"
	
	location "asyncmos"
	objdir "asyncmos/intermediate"
	
	flags { "NoPCH", "MultiProcessorCompile" }
	defines({
		"_CRT_NONSTDC_NO_WARNINGS",
		"_CRT_SECURE_NO_WARNINGS",
		"STRICT"
	})
	vectorextensions "SSE2"
	configurations { "Release", "Debug" }
	
	filter "configurations:Release"
		symbols "Off"
		optimize "Speed"
		floatingpoint "Fast"
		omitframepointer "On"
		flags { "LinkTimeOptimization" }
		buildoptions { "/Qpar", "/GR-", "/Gw" }
		
		defines "NDEBUG"
		targetdir "asyncmos/release"
	
	filter "configurations:Debug"
		symbols "Full"
		optimize "Debug"
		
		defines { "DEBUG", "_DEBUG" }
		targetdir "asyncmos/debug"
	
	filter {}
	
	project "asyncmos"
		files { "source/**.h", "source/**.cpp" }
		vpaths {
			["Header Files/*"] = "source/**.h",
			["Source Files/*"] = "source/**.cpp"
		}
