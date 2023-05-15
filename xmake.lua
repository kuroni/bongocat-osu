set_project("bongocat-osu")
set_version("1.5.5")

set_xmakever("2.7.5")

add_rules("mode.debug", "mode.release")

set_warnings("all")
set_languages("c++20")

if is_mode("debug") then
    set_symbols("debug")
    set_optimize("none")
    set_runtimes("MDd")
    set_policy("build.warning", true)
elseif is_mode("release") then
    set_strip("all")
    set_symbols("hidden")
    set_optimize("smallest")
    set_runtimes("MT")
    add_defines("NDEBUG")
end

-- use dynamic libraries acceleration on linking
if is_mode("debug") then
    add_requireconfs("*", {configs = {shared = true}})
end

add_requires("sfml 2.5.1", "jsoncpp 1.9.5")
-- TODO: Linux test
if is_plat("linux") then
    -- add_requires("sdl2", "libx11", "libxdo")
end

target("bongo")
    set_kind("binary")
    add_files("src/*.cpp")
    add_files("ico/ico.rc")

    add_packages("sfml", "jsoncpp")

    set_rundir("$(projectdir)")
    
    on_load(function (target)
        if is_plat("windows") then
            target:add("links", "user32")
        elseif is_plat("linux") then
            -- TODO: Linux test
            -- target:add("packages", "libx11", "libxdo")
            -- target:add("links", "X11", "xdo", "Xrandr")
        else
            assert(false, "Not support plat!")
        end
    end)

    on_install(function (target)
        local outputdir = "bin"
        os.mkdir(outputdir)
        os.cp("$(projectdir)/font", outputdir)
        os.cp("$(projectdir)/img", outputdir)
        os.cp(target:targetfile(), outputdir)

        import("utils.archive")
        local archive_name = format("bongocat-osu_%s_%s.zip", target:plat(), target:arch())
        archive.archive(archive_name, "bin")
    end)

    on_uninstall(function (target)
        local archive_name = format("bongocat-osu_%s_%s.zip", target:plat(), target:arch())
        os.tryrm("bin")
        os.tryrm(archive_name)
    end)
