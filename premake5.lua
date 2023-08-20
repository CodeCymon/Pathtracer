-- premake5.lua
workspace "Pathtracing"
   architecture "x64"
   configurations { "Debug", "Release", "Dist" }
   startproject "Pathtracing"

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"
include "Walnut/WalnutExternal.lua"

include "Pathtracing"