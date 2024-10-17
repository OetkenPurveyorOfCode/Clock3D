import os
import sys
import subprocess as sub

modelnames = sorted([
    "back_frame",
    "front_frame",
    "pallet",
    "escapement",
    "gear2_54_12",
    "gear3_54_12",
    "gear4_54_18",
    "gear7_48",
    "gear7_24",
    "gear7_clicks",
    "gear8_54",
    "gear5_48_12",
    "gear4_18",
    "gear6_54",
    "minute_hand",
    "hour_hand",
])

sub.run(("gcc", "-o", "stlembed.exe", "stlembed.c"))
for modelname in modelnames: 
    sub.run((
        "stlembed", 
        f"../assets_stl/{modelname}.stl", 
        f"../baked/{modelname}.h",
        modelname
    ))

sub.run(("gcc", "-o", "cembed.exe", "cembed.c"))
sub.run(("cembed", "-q", "-o", "../baked/lighting.vs.h", "../assets_shaders/lighting.vs"))
sub.run(("cembed", "-q", "-o", "../baked/lighting.fs.h", "../assets_shaders/lighting.fs"))

sub.run(("cembed", "-q", "-o", "../baked/lighting_es.vs.h", "../assets_shaders/lighting_es.vs"))
sub.run(("cembed", "-q", "-o", "../baked/lighting_es.fs.h", "../assets_shaders/lighting_es.fs"))

sub.run(("cembed", "-o", "../baked/tick.h", "../assets_sounds/tick.wav"))
sub.run(("cembed", "-o", "../baked/tock.h", "../assets_sounds/tock.wav"))



