#!/usr/bin/env python3

import os
import sys
import argparse


def _log_process(msg):
    print("[PROCESS:util/load_dataset_2_config.py] {}".format(msg))
def _log_info(msg):
    print("\033[95m[INFO:util/load_dataset_2_config.py] {}\033[00m".format(msg))
def _log_error(msg):
    print("\033[91m[ERROR:util/load_dataset_2_config.py] {}\033[00m".format(msg))
def _log_warning(msg):
    print("\033[93m[WARNING:util/load_dataset_2_config.py] {}\033[00m".format(msg))
def _log_success(msg):
    print("\033[92m[SUCCESS:util/load_dataset_2_config.py] {}\033[00m".format(msg))


def main(source_path: str, config_path: str) -> None:
    _log_process("Loading the labels file...")

    try:
        classifier_labels_path_key: str = 'classifierLabelsPath:'
        classifier_labels: str = 'classifierLabels:'
        with open(config_path, "r") as f:
            config_lines = f.readlines()

        labels_path = None
        for line in config_lines:
            if classifier_labels_path_key in line:
                _, labels_path = line.split(':', 1)
                labels_path = labels_path.strip().strip('\"')
                break

        if not labels_path:
            raise ValueError(f"{classifier_labels_path_key} not found in the configuration file")

        with open(os.path.join(source_path, labels_path), "r") as f:
            labels = [line.strip() for line in f if line.strip()]

        start_idx = -1
        end_idx = -1
        for i, line in enumerate(config_lines):
            if line.strip() == classifier_labels:
                start_idx = i
                end_idx = start_idx + 1
                while end_idx < len(config_lines) and config_lines[end_idx].strip().startswith('-'):
                    end_idx += 1
                break

        new_labels_section = [f'{classifier_labels}\n'] + \
                             [f"   - \"{label}\"\n" for label in labels]

        if start_idx != -1:
            config_lines = config_lines[:start_idx] + new_labels_section + config_lines[end_idx:]
        else:
            config_lines.extend(['\n'] + new_labels_section)

        with open(config_path, "w") as f:
            f.writelines(config_lines)

    except FileNotFoundError as e:(
        _log_error(f"Error: File not found - {e}"))
    except ValueError as e:(
        _log_error(f"Error: {e}"))
    except Exception as e:(
        _log_error(f"An unexpected error occurred: {e}"))



if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Load the labels into the TTool config.yml file.")
    parser.add_argument("-s", "--source", help="Path to the project source directory.")
    parser.add_argument("-c", "--config", help="Path to the TTool config.yml file.")

    args = parser.parse_args()

    if not os.path.isdir(args.source):
        _log_error("The path to the project source directory is not valid.")
        sys.exit()
    if not os.path.isfile(args.config):
        _log_error("The path to the config file is not valid.")
        sys.exit()

    main(source_path=args.source, config_path=args.config)
