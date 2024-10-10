import os
import subprocess

project_dir = os.environ.get('PROJECTDIR')

if os.name == 'nt':
    glslc_executable = "glslc.exe"
else:
    glslc_executable = "glslc"

print("Shader builder working directory: " + project_dir)

for filename in os.listdir(project_dir + "/shaders/"):
    if filename.endswith(".vert") or filename.endswith(".frag") or filename.endswith(".comp"):
        shader_file_path = os.path.join(project_dir + "/shaders/", filename)

        output_file = os.path.splitext(shader_file_path)[0] + os.path.splitext(shader_file_path)[1] + ".spv"

        command = [glslc_executable, shader_file_path, "-o", output_file]

        try:
            subprocess.run(command, check=True)
            print(f"Compiled {filename} to {output_file}")
        except subprocess.CalledProcessError as e:
            print(f"Failed to compile {filename}: {e}")
