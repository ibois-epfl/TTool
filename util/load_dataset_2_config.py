#!/usr/bin/env python

import os
import sys
import argparse

def _log_process(msg):
    print("[PROCESS:util/load_dataset_2_config.py] {}".format(msg))
def _log_info(msg):
    # print in violet
    print("\033[95m[INFO:util/load_dataset_2_config.py] {}\033[00m".format(msg))
def _log_error(msg):
    print("\033[91m[ERROR:util/load_dataset_2_config.py] {}\033[00m".format(msg))
def _log_warning(msg):
    print("\033[93m[WARNING:util/load_dataset_2_config.py] {}\033[00m".format(msg))
def _log_success(msg):
    print("\033[92m[SUCCESS:util/load_dataset_2_config.py] {}\033[00m".format(msg))

def get_keys_yml(path : str) -> dict:
    dict_keys_lnnbr : dict = {}
    with open(path, "r") as f:
        lines = f.readlines()
        for i in range(len(lines)):
            if i < 3:
                continue
            if lines[i][0] != "\t" and \
             lines[i][0] != " " and \
             lines[i][0] != "#" and \
             lines[i][0] != "\n" and \
             lines[i][0] != "-" and \
             lines[i][0] != "[" and \
             lines[i][0] != "]" and \
             lines[i][0] != ":" and \
             lines[i][0] != '"""':
                dict_keys_lnnbr[lines[i].strip().split(':')[0]] = i
    return dict_keys_lnnbr

def main(dataset_path : str, config_path : str) -> None:
    _log_process("Loading the dataset paths...")

    # count the number of subfolders in the dataset path
    num_folders : int = 0
    for root, dirs, files in os.walk(dataset_path):
        num_folders += len(dirs)
    _log_info("Found {} folders.".format(num_folders))
    model_obj_paths : list[str] = []
    model_init_pose_paths : list[str] = []
    for root, dirs, files in os.walk(dataset_path):
        for file in files:
            if file.endswith(".obj"):
                model_obj_paths.append(os.path.join(root, file))
            if file.endswith(".initpose"):
                model_init_pose_paths.append(os.path.join(root, file))
    _log_info("Found {} .initpose files.".format(len(model_init_pose_paths)))
    _log_info("Found {} .obj files.".format(len(model_obj_paths)))
    print(f"model_obj_paths: {model_obj_paths}")
    print(f"num_folders: {num_folders}")
    assert len(model_obj_paths) == num_folders, "The number of .obj files is not equal to the number of folders.\
        \nPlease make sure that each folder has a .obj file."
    assert len(model_init_pose_paths) == num_folders, "The number of .initpose files is not equal to the number of folders.\
        \nPlease make sure that each folder has a .initpose file."
    _log_success("Obj paths found.")

    _log_process("Parsing/Modifying the config file with dataset paths and reset poses...")
    key_modelFiles : str = "modelFiles"
    key_resetPoses : str = "groundTruthPoses"

    dict_keys_lnnbr : dict = get_keys_yml(config_path)
    if key_modelFiles not in dict_keys_lnnbr.keys() or \
        key_resetPoses not in dict_keys_lnnbr.keys():
            _log_error("The key 'modelFiles' or 'groundTruthPoses' is not found in the config file.")
            sys.exit()
    
    # cleaning
    key_modelFiles_IDX = list(dict_keys_lnnbr.keys()).index(key_modelFiles)
    key_resetPoses_IDX = list(dict_keys_lnnbr.keys()).index(key_resetPoses)
    try:
        key_modelFiles_IDX_NEXT = list(dict_keys_lnnbr.keys())[key_modelFiles_IDX + 1]
    except IndexError:
        key_modelFiles_IDX_NEXT = None
    try:
        key_resetPoses_IDX_NEXT = list(dict_keys_lnnbr.keys())[key_resetPoses_IDX + 1]
    except IndexError:
        key_resetPoses_IDX_NEXT = None
    with open(config_path, "r") as f:
        lines = f.readlines()
        for i in range(len(lines)):
            if key_modelFiles_IDX_NEXT != None:
                if dict_keys_lnnbr[key_modelFiles] < i < dict_keys_lnnbr[key_modelFiles_IDX_NEXT]:
                    lines[i] = ""
            else:
                if dict_keys_lnnbr[key_modelFiles] < i:
                    lines[i] = ""
            if key_resetPoses_IDX_NEXT != None:
                if dict_keys_lnnbr[key_resetPoses] < i < dict_keys_lnnbr[key_resetPoses_IDX_NEXT]:
                    lines[i] = ""
            else:
                if dict_keys_lnnbr[key_resetPoses] < i:
                    lines[i] = ""
    with open(config_path, "w") as f:
        f.writelines(lines)
    
    dict_keys_lnnbr : dict = get_keys_yml(config_path)

    for key in dict_keys_lnnbr.keys():
        print("{}: {}".format(key, dict_keys_lnnbr[key]))

    new_lines_modelFiles : list[str] = []
    new_lines_gtPoses : list[str] = []
    for i in range(len(model_obj_paths)):
        # load the model path from the file
        new_model_path : str = ""
        new_lines_modelFiles.append("   - \"{}\"\n".format(model_obj_paths[i]))

        # load the pose from the file
        with open(model_init_pose_paths[i], "r") as f:
            lines = f.readlines()
            for line in lines:
                new_model_path += line
        new_lines_gtPoses.append("{}\n".format(new_model_path))

    with open(config_path, "r+") as f:
        lines = f.readlines()
        f.seek(0)
        f.truncate()

        for i in range(len(lines)):
            if i == (dict_keys_lnnbr[key_resetPoses]):
                f.write(lines[i])
                f.writelines(new_lines_gtPoses)
                continue
            if i == (dict_keys_lnnbr[key_modelFiles]):
                f.write(lines[i])
                f.writelines(new_lines_modelFiles)
                continue
            f.write(lines[i])
    _log_success("Config file modified.")


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Load the dataset paths into the TTool config .yml file.")
    parser.add_argument("-p", "--path", help="Path to the dataset folder.")
    parser.add_argument("-c", "--config", help="Path to the TTool config .yml file.")

    args = parser.parse_args()

    if not os.path.isdir(args.path):
        _log_error("The path to the dataset is not valid.")
        sys.exit()
    if not os.path.isfile(args.config):
        _log_error("The path to the config file is not valid.")
        sys.exit()

    main(dataset_path=args.path, config_path=args.config)