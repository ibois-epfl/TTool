import os


def create_out_dir(out_path):
    script_dir = os.path.dirname(os.path.abspath(__file__))
    default_path = os.path.abspath(os.path.join(script_dir, ".."))  # Go up one level from the script directory
    base_out_dir = os.path.join(out_path if out_path else default_path, 'out')

    # Create subdirectories for 'csv', 'latex', and 'boxplot'
    subdirs = ['csv', 'latex', 'boxplot']
    paths = {}
    for subdir in subdirs:
        dir_path = os.path.join(base_out_dir, subdir)
        os.makedirs(dir_path, exist_ok=True)
        paths[subdir] = dir_path

    return paths

