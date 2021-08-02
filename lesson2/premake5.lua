project 'lesson2'
  kind 'ConsoleApp'
  language 'C++'
  staticruntime 'On'
  cppdialect 'C++17'

  targetdir("%{wks.location}/bin/%{cfg.buildcfg}")
  objdir("%{wks.location}/obj/%{cfg.buildcfg}/%{prj.name}")

  dependson {
    "common"
  }

  defines {}

  files {
    'src/**.h',
    'src/**.hpp',
    'src/**.cpp',

    STB_SRC_FILES
  }

  includedirs {
    'src',

    COMMON_INCLUDE,
    VENDOR_INCLUDE
  }

  filter "system:macosx"
    system "macosx"

  filter "system:linx"
    system "linux"

  filter "system:windows"
    system "windows"

  filter "configurations:debug"
    defines {"_DEBUG"}
    symbols "On"

  filter "configurations:release"
    defines {"_RELEASE"}
    optimize "On"

