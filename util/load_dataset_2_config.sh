#!/bin/bash

function print_process {
  echo -e "\e[37m[Process:util/load_dataset_2_config.sh]: $1 \e[0m"
}
function print_info {
  echo -e "\e[35m[Info:util/load_dataset_2_config.sh]: $1 \e[0m"
}
function print_error {
  echo -e "\e[31m[Error:util/load_dataset_2_config.sh]: $1 \e[0m"
}
function print_warning {
  echo -e "\e[33m[Warning:util/load_dataset_2_config.sh]: $1 \e[0m"
}
function print_success {
  echo -e "\e[32m[Success:util/load_dataset_2_config.sh]: $1 \e[0m"
}


__workin_dir__="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"
  print_process "Changing directory to root"
  cd $__workin_dir__ && cd ../

__config_ttol_file__=$1
if [ ! -f "$__config_ttol_file__" ]; then
  print_error "Config file $__config_ttol_file__ does not exist"
  exit 1
fi
print_info "----------------------------------------"
print_info "Config file content:"
cat $__config_ttol_file__
print_info "----------------------------------------"
print_success "Config file loaded successfully"

__dataset_dir__=$2
if [ ! -d "$__dataset_dir__" ]; then
  print_process "Creating directory $__dataset_dir__"
  mkdir -p $__dataset_dir__
fi
if [ "$(ls -A $__dataset_dir__)" ]; then
  print_warning "Directory $__dataset_dir__ is not empty, exiting.."
fi




# ====================
# # for each folder in dataset directory, look into the folder and get the path of the object file .obj
# print_process "Loading dataset to config file..."
# for folder in $__dataset_dir__/*; do
#   print_process "----------------------------------------"
#   print_info "Loading $folder to config file..."
#   for file in $folder/*; do
#     if [[ $file == *.obj ]]; then
#       print_info "Found $file"
      
#     #   echo $file >> $__config_ttol_file__
#       print_success "$file added to config file"
#     fi
#   done
#   print_success "$folder loaded to config file"
#   print_process "----------------------------------------"
# done