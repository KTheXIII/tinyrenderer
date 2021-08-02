COMMON_INCLUDE = path.getabsolute("include")
VENDOR_INCLUDE = path.getabsolute("vendor")

STB_SRC_FILES  = VENDOR_INCLUDE .. "/stb/**.cpp"

project "common"
  language "C++"
  cppdialect "C++17"
  kind "Utility"

  defines {}

  files {
    "include/**.h",
    "include/**.hpp",

    "vendor/**.h",
    "vendor/**.hpp",
  }

  includedirs {
    "include",
    VENDOR_INCLUDE
  }

  links {}

  filter "system:macosx"
    system "macosx"
    systemversion "latest"

  filter "system:linux"
    system "linux"
    systemversion "latest"

  filter "system:windows"
    system "windows"
    systemversion "latest"

