#!/bin/bash


BOLD='\033[1m'
RESET='\033[0m'
BG_BLUE='\033[44m'

function print_error() {
  echo -e "\033[0;31m[ERROR] $1\033[0m";
}
function print_info() {
  echo -e "\033[0;90m[INFO] $1\033[0m";
}
function print_warning() {
  echo -e "\033[0;33m[WARNING] $1\033[0m";
}
function print_prompt() {
  echo -e "\033[0;36m[PROMPT] $1\033[0m";
}

function run_bg() {
  echo -en "\033[0;90m"
  $@
  echo -e "\033[0m"
}

# Parse the arguments
DATA_PATH=""
OUT_PATH=""

while [ "$#" -gt 0 ]; do
  case "$1" in
    --data_path)
      DATA_PATH="$2"
      shift 2
      ;;
    --out_path)
      OUT_PATH="$2"
      shift 2
      ;;
    *)
      print_error "Unknown argument: $1"
      exit 1
      ;;
  esac
done

# Check if the variables are set
if [ -z "$DATA_PATH" ] || [ -z "$OUT_PATH" ]; then
    print_error "Missing arguments. Usage: $0 --data_path DATA_PATH --out_path OUT_PATH"
    exit 1
fi

# Set the path to the conda executable
CONDA_PATH=$(which conda)

if [ -z "$CONDA_PATH" ]; then
    print_error "Conda command not found. Make sure Anaconda or Miniconda is installed and in your PATH."
    exit 1
fi


# Path to the directory containing the environment.yml file
ENV_YML_PATH="environment.yml"
# Extract the environment name from the YAML file
ENV_NAME=$(grep 'name:' $ENV_YML_PATH | awk '{print $2}')
if [ -z "$ENV_NAME" ]; then
    print_error "Could not find the environment name in $ENV_YML_PATH."
    exit 1
fi

print_info "Environment name: $ENV_NAME"

# Check if the environment already exists
env_exists=$(conda info --envs | grep "$ENV_NAME")
if [ -n "$env_exists" ]; then
    print_warning "The Conda environment '$ENV_NAME' already exists."
    print_prompt "Would you like to update the environment? [y/N]: }"
    read update_choice
    case $update_choice in
        [Yy]* )
            print_info "Updating the Conda environment '$ENV_NAME'."
            run_bg conda env update -f "$ENV_YML_PATH" --prune
            ;;
        * )
            print_info "Not updating the existing Conda environment."
            ;;
    esac
else
    print_info "Creating the Conda environment '$ENV_NAME'."
    run_bg conda env create -f "$ENV_YML_PATH"
fi

# Activate the environment
conda activate "$ENV_NAME"
echo -e "${BOLD}${BG_BLUE}[INFO] Activated the Conda environment '$ENV_NAME'.${RESET}"

# Run the evaluation script
print_info "Running the evaluation script."
python script/compute.py --data_path "$DATA_PATH" --out_path "$OUT_PATH"

