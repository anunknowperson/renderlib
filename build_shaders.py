import os
import subprocess
import shutil


<<<<<<< HEAD:build_shaders.py
project_dir = os.getcwd()
=======
project_dir = os.environ.get('PROJECTDIR')
if not project_dir:
    raise ValueError("Environment variable PROJECTDIR is not set")

>>>>>>> 1fc42e5 (Fixed the shader build script: moved copy logic to Python, added directory creation, simplified CMake, updated compileShaders.cmake, added error handling):scripts/build_shaders.py

if os.name == 'nt':
    glslc_executable = "glslc.exe"
else:
    glslc_executable = "glslc"


shaders_source_dir = os.path.join(project_dir, "shaders")
shaders_dest_dir = os.path.join(project_dir, "build", "shaders")


os.makedirs(shaders_dest_dir, exist_ok=True)

print("Shader builder working directory: " + project_dir)


for filename in os.listdir(shaders_source_dir):
    if filename.endswith(".vert") or filename.endswith(".frag") or filename.endswith(".comp"):
        shader_file_path = os.path.join(shaders_source_dir, filename)

        
        output_file = os.path.splitext(shader_file_path)[0] + os.path.splitext(shader_file_path)[1] + ".spv"

        
        command = [glslc_executable, shader_file_path, "-o", output_file]

        try:
           
            subprocess.run(command, check=True)
            print(f"Compiled {filename} to {output_file}")

            
            dest_file = os.path.join(shaders_dest_dir, os.path.basename(output_file))
            shutil.copy(output_file, dest_file)
            print(f"Copied {output_file} to {dest_file}")

        except subprocess.CalledProcessError as e:
            print(f"Failed to compile {filename}: {e}")
        except Exception as e:
            print(f"Failed to copy {output_file}: {e}")