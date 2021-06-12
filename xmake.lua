add_rules("mode.debug")
add_rules("mode.release")

set_toolchains("clang")

--set_languages("c++14")
--set_languages("c++17")

target("cxxspec")
    set_kind("shared")
    add_files("src/*.cpp", "src/**/*.cpp")
    add_headerfiles("src/*.hpp", "src/(**/*.hpp)", {prefixdir = "cxxspec"})
    add_includedirs("src", {public = true})

target("specs")
    set_default(false)
    set_kind("binary")
    add_deps("cxxspec")
    add_files("spec/*.cpp")