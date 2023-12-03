import os
import subprocess


shaders_directory = os.environ['PYTHONPATH'] + "/shaders/"


glslc_executable = "glslc.exe"


print("Shader builder working directory: " + os.environ['PYTHONPATH'])
for filename in os.listdir(shaders_directory):
    if filename.endswith(".vert") or filename.endswith(".frag"):
        shader_file_path = os.path.join(shaders_directory, filename)
        output_file = os.path.splitext(shader_file_path)[0] + os.path.splitext(shader_file_path)[1] + ".spv"


        command = [glslc_executable, shader_file_path, "-o", output_file]

        try:
            subprocess.run(command, check=True)
            print(f"Compiled {filename} to {output_file}")
        except subprocess.CalledProcessError as e:
            print(f"Failed to compile {filename}: {e}")