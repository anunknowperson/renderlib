import os
import subprocess
import argparse

def compile_shaders(source_dir: str, output_dir: str):
    """Compile GLSL shaders to SPIR-V binaries."""
    glslc = "glslc.exe" if os.name == "nt" else "glslc"
    os.makedirs(output_dir, exist_ok=True)

    # Process all shader files recursively
    for root, _, files in os.walk(source_dir):
        for filename in files:
            if not filename.endswith((".vert", ".frag", ".comp")):
                continue

            input_path = os.path.join(root, filename)
            output_name = f"{os.path.splitext(filename)[0]}.spv"
            output_path = os.path.join(output_dir, output_name)

            try:
                subprocess.run([glslc, input_path, "-o", output_path], check=True)
                print(f"[OK] {filename} → {output_name}")
            except subprocess.CalledProcessError as e:
                print(f"[ERROR] {filename}: {e}")
            except Exception as e:
                print(f"[FATAL] {filename}: {e}")

if __name__ == "__main__":
    # Configure command-line arguments
    parser = argparse.ArgumentParser(description="GLSL to SPIR-V compiler")
    parser.add_argument("--source", required=True, help="Source shaders directory")
    parser.add_argument("--output", required=True, help="Output directory (build/demo)")
    args = parser.parse_args()
    
    compile_shaders(args.source, args.output)