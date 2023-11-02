import os

def count_lines_in_file(file_path):
    try:
        with open(file_path, 'r') as file:
            return sum(1 for line in file)
    except Exception as e:
        print(f"Error reading {file_path}: {e}")
        return 0

def count_lines_in_directory(directory):
    total_lines = 0
    for root, dirs, files in os.walk(directory):
        for file in files:
            file_path = os.path.join(root, file)
            total_lines += count_lines_in_file(file_path)
    return total_lines

if __name__ == "__main__":
    src_directory = "src/"
    if not os.path.exists(src_directory):
        print(f"The directory {src_directory} does not exist.")
    else:
        total_lines = count_lines_in_directory(src_directory)
        print(f"Total number of lines in all files in {src_directory}: {total_lines}")
    
    input("Press Enter to exit...")
