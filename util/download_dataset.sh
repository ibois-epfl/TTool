#!/bin/bash

function print_process {
  echo -e "\e[37m[Process:util/download_dataset.sh]: $1 \e[0m"
}
function print_info {
  echo -e "\e[35m[Info:util/download_dataset.sh]: $1 \e[0m"
}
function print_error {
  echo -e "\e[31m[Error:util/download_dataset.sh]: $1 \e[0m"
}
function print_warning {
  echo -e "\e[33m[Warning:util/download_dataset.sh]: $1 \e[0m"
}
function print_success {
  echo -e "\e[32m[Success:util/download_dataset.sh]: $1 \e[0m"
}

__workin_dir__="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"
print_process "Changing directory to root"
cd $__workin_dir__ && cd ../

__zenodo_url__=$1
if [ -z "$__zenodo_url__" ]; then
  print_error "Please provide the Zenodo URL"
  exit 1
fi
__dataset_dir__=$2
if [ ! -d "$__dataset_dir__" ]; then
  print_process "Creating directory $__dataset_dir__"
  mkdir -p $__dataset_dir__
fi
if [ "$(ls -A $__dataset_dir__)" ]; then
  print_warning "Directory $__dataset_dir__ is not empty, exiting.."
  exit 1
fi

print_process "Downloading dataset ttool.."
html_page=$(wget -qO- $__zenodo_url__)
download_links=$(echo $html_page | grep -oP 'href="\Khttps?://[^"]+')
download_links=$(echo $download_links | tr " " "\n" | grep -oP 'https?://[^"]+\.zip')

for link in $download_links; do
    print_process "----------------------------------------"
    filename_zip=$(basename $link)
    echo $filename_zip
    
    print_info "Downloading $filename_zip... to $__dataset_dir__"
    wget -O $__dataset_dir__/$filename_zip $link
    print_success "$filename_zip downloaded"
    print_process "extracting $filename_zip..."
    unzip -o $__dataset_dir__/$filename_zip -d $__dataset_dir__
    rm $__dataset_dir__/$filename_zip
    print_success "$filename_zip extracted"
    print_process "----------------------------------------"
done
print_success "Dataset downloaded successfully"
