import os
import subprocess
import sys

project_dir = os.environ.get('PROJECTDIR') # Get project directory from environment variable

if len(sys.argv) > 1:
    build_dir = sys.argv[1]  # Use command-line argument as build directory
else:
    print("Error: Build directory not specified")
    sys.exit(1)

os.makedirs(build_dir, exist_ok=True) # Create build directory

glslc_executable = "glslc.exe" if os.name == 'nt' else "glslc" # Determine glslc executable name

print("Project dir: " + project_dir)
print("Build dir: " + build_dir)

shaders_dir = os.path.join(project_dir, "shaders") # Path to shaders directory

for filename in os.listdir(shaders_dir): # Iterate through files in shaders directory
    if filename.endswith((".vert", ".frag", ".comp")): # Check for shader file extensions
        shader_path = os.path.join(shaders_dir, filename) # Full path to input shader
        output_path = os.path.join(build_dir, filename + ".spv") # Full path to output SPIR-V file

        try:
            subprocess.run([glslc_executable, shader_path, "-o", output_path], check=True) # Run glslc compiler
            print(f"Compiled {filename} to {output_path}") # Success message
        except subprocess.CalledProcessError as e:
            print(f"Failed to compile {filename}: {e}") # Error message on compilation failure

