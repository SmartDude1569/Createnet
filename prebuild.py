import re
import os

version_file = "version.h"

if not os.path.exists(version_file):
    print(f"{version_file} not found!")
    exit(1)

with open(version_file, "r") as file:
    lines = file.readlines()

new_lines = []
build_updated = False

for line in lines:
    if "#define BUILD_NUMBER" in line:
        m = re.search(r"(\d+)", line)
        if m:
            current_number = int(m.group(1))
            new_number = current_number + 1
            new_line = f"#define BUILD_NUMBER  {new_number}\n"
            new_lines.append(new_line)
            print(f"Updated BUILD_NUMBER to {new_number}")
            build_updated = True
        else:
            new_lines.append(line)
    else:
        new_lines.append(line)

if not build_updated:
    print("No BUILD_NUMBER line found.")
else:
    with open(version_file, "w") as file:
        file.writelines(new_lines)