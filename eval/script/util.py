import os


def create_out_dir(out_path: str) -> dict[str, str, str]:
    """
        Creates the output directories for the csv, latex and boxplot files

        Args:
            out_path (str): The path to the output directory
        Returns:
            dict[str, str, str]: The paths of the output directories
    """
    subdirs = ['csv', 'latex', 'boxplot']
    paths = {}
    out_path = os.path.join(out_path, 'output')
    for subdir in subdirs:
        dir_path = os.path.join(out_path, subdir)
        try:
            os.makedirs(dir_path, exist_ok=True)
            paths[subdir] = dir_path
        except OSError as e:
            print(f"\033[91m[ERROR]: Could not create directories. Error:{e}\033[0m")
            raise e
    return paths

