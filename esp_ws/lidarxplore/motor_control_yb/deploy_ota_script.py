import os, shutil

# Get the folder where this script is located
script_dir = os.path.dirname(os.path.abspath(__file__))

# Build absolute path to .pio
source_file = os.path.join(script_dir, ".pio", "build", "esp32-s3-devkitc-1", "firmware.bin")
destination_file = os.path.join(script_dir, "deploy", "firmware.bin")

# Clean deploy folder before writing new files
if os.path.exists(os.path.dirname(destination_file)):
    print(f"[POST-BUILD] Cleaning old deploy folder: {os.path.dirname(destination_file)}")
    shutil.rmtree(os.path.dirname(destination_file))

# Ensure destination folder exists
os.makedirs(os.path.dirname(destination_file), exist_ok=True)

# Copy file
shutil.copy(source_file, destination_file)
print(f"Copied: {source_file} -> {destination_file}")