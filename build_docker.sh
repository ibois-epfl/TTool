#!/bin/bash
# delete all cmake cache files 
find . -name "CMakeCache.txt" -type f -delete

sudo docker build -t ttool .