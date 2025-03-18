#!/usr/bin/python3
import os
import argparse

#========================================================
# get the existing project's name
def cmake_get_project_name(path_name):
    f = open(path_name, "r")
    lines = f.readlines()
    f.close()
    for line in lines:
        pos = line.find("project(")
        if(pos != -1):
            last_pos = line.find(" ", pos + 8)
            return line[pos + 8:last_pos]
    return "null"

def replace_string_in_file(path, old, new):
    f_read = open(path, "r")
    lines = f_read.readlines()
#    print(f"Read {len(lines)} lines from file '{path}'")
    f_read.close()
    f_write = open(path, "w")
    linesWritten = 0
    for l in lines:
        f_write.write(l.replace(old, new))
        linesWritten += 1

    f_write.close()
    #print(f"Wrote {linesWritten} lines to file '{path}'")

#========================================================
parser = argparse.ArgumentParser(description="Provide your plugin's new name")
parser.add_argument("new_name", help="The new name")
args = parser.parse_args()

print(f"Changing plugin's name to {args.new_name}. . .")

# 1. get the previous name
prev_name = cmake_get_project_name("plugin/CMakeLists.txt")

print(f"Found previous name '{prev_name}'. . .")

# 2. rename the include directory w the new plugin
prev_inc_dir = "plugin/include/" + prev_name
new_inc_dir = "plugin/include/" + args.new_name
os.rename(prev_inc_dir, new_inc_dir)

# 3. replace the old name with the new in each of the relevant files
print("Updating cmake file")
replace_string_in_file("plugin/CMakeLists.txt", prev_name, args.new_name)
print("Updating header files")
replace_string_in_file(new_inc_dir + "/PluginEditor.h", prev_name, args.new_name)
replace_string_in_file(new_inc_dir + "/PluginProcessor.h", prev_name, args.new_name)
replace_string_in_file(new_inc_dir + "/Identifiers.h", prev_name, args.new_name)
print("Updating source files")
replace_string_in_file("plugin/source/PluginEditor.cpp", prev_name, args.new_name)
replace_string_in_file("plugin/source/PluginProcessor.cpp", prev_name, args.new_name)
replace_string_in_file("plugin/source/Common.cpp", prev_name, args.new_name)
replace_string_in_file("plugin/source/Identifiers.cpp", prev_name, args.new_name)
print("Updating test files")
replace_string_in_file("test/source/AudioProcessorTest.cpp", prev_name, args.new_name)
print("Name change finished!")
