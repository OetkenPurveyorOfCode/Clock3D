import os
import sys
import subprocess as sub


sub.run(("gcc", "-o", "stl2obj.exe", "stl2obj.c"))
for lfile in os.listdir("../assets_stl/"):
    path = os.path.join("../assets_stl/", lfile)
    lfile_without_ext, _ = os.path.splitext(lfile)
    outpath  = os.path.join("../assets_obj/", lfile_without_ext+".obj")
    print(path, outpath)
    sub.run(("stl2obj", path, outpath))
