add_rules("mode.debug")
add_rules("mode.release")

add_requires("cxxspec")

target("simple")
    set_kind("binary")
    add_packages("cxxspec")
    add_files("simple.cpp")