import os


def create_out_directory(out_path):
    script_dir = os.path.dirname(os.path.abspath(__file__))
    default_path = os.path.abspath(os.path.join(script_dir, "..", ".."))
    if out_path is not None:
        out_dir = os.path.join(out_path, 'out')
    else:
        out_dir = os.path.join(default_path, 'out')
    os.makedirs(out_dir, exist_ok=True)
    return out_dir

