# TTool evaluation
This folder contains all the necessary files and scripts to reproduce the evaluation of the TTool

## How to Run
```bash
source evaluate.sh --data_path /home/user/coordinates.log --out_path ./output
```
- `--data_path`: root of log file with coordinates from AC
- `--out_path`: root of the export da

or 

### Anaconda environment
Install the python dependencies in Anaconda and run it from there (see `environment.yml`).

### Results for pose estimation position and rotation errors
```bash
python3 compute.py --data_path  /home/user/coordinates.log --out_path ../output
```
- `--data_path`: root of log file with coordinates from AC
- `--out_path`: root of the export data

