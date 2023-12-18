# TTool evaluation
This folder contains all the necessary files and scripts to reproduce the evaluation of the TTool. 
Below is an overview of the key components and the results you can expect from this evaluation.

# Contents Overview
- `script/compute.py`: Main script that runs the evaluation. 
- `script/compute_error.py`: Contains functions to compute the position and rotation errors between the ground truth and the estimated pose.
- `script/compute_ux.py`: Contains the UX evaluation results and its categories for the graph, also exports the result to a graph. 
   **Note**: The UX evaluation results are not included in the AC log file, they are provided separately and hardcoded in this script.
- `script/io_stream`: Includes functions to read the AC log file and export the results to CSV.
- `script/visuals.py`: Provides functions to plot the evaluation results and export the LaTeX tables.
- `script/util.py`: Includes the utility function to create the output directories

# Evaluation Output
The evaluation process yields detailed insights into the performance and accuracy of the TTool:
- **Position Error Analysis**:
    - Measures the euclidean distance between two points in 3D space: the **base** and **tip** of the toolhead (estimated pose), 
and the hole (that serves as the ground truth). It also computes the **mean** position error across these measurements.
- **Rotation Error Analysis**:
    - Measures the **angle** between the two vectors of the toolhead (estimated pose) and the hole (ground truth).
- **UX Evaluation**:
    - Assesses the user experience of the TTool using the `User Experience Questionnaire Results.txt` results. 

# Results Format
- The results of the evaluation are presented in various formats for ease of analysis and reporting:
    - **Boxplot Graphs**: These visual representations not only show error distributions but also include key statistical 
data such as max, min, mean, median, standard deviation, and quartiles (Q1, Q3).
    - **CSV Files**: Detailed numerical data for in-depth analysis. 
    - **LaTeX Tables**: Summarize the statistical data in a format ready for documentation and presentations. 
These tables include the same statistical measures as presented in the boxplots and CSVs for consistency and ease of comparison.
    - **Progression Graphs**: Visualize the progression of the error over the cycles of the evaluation.
    - **UX Graphs**: Visualize the UX evaluation results.

    
# Prerequisites
Anaconda Installation: Ensure that Anaconda is installed on your system. Anaconda is required for creating and managing the environment where the evaluation will be run. 
For more information on Anaconda, see the [Anaconda website](https://www.anaconda.com/).

# Getting Started
There are two main ways to run the evaluation: using the provided evaluate.sh script, or manually setting up the environment and running the scripts.

## Automated Setup 

### 1. Using evaluate.sh Script

Use the following command to automatically set up the Anaconda environment and run the evaluation:
```bash
source evaluate.sh --data_path /home/user/session1.log --out_path . --dir_name session1
```
- `--data_path`: Specifies the path to the log file containing coordinates from AC.
- `--out_path`: Defines the path for the exported data.
- `--dir_name`: Defines the name of the directory in the `output` directory.

This script will check if Anaconda is installed and if the required environment exists. If the environment exists, you'll be prompted to update it. 
If not, it will be created using the `environment.yml` file. 
Then it will run the evaluation and export the results to the **/output** directory within the specified path under the given directory name.

## Manual Setup
For a manual setup, first create the Anaconda environment using the `environment.yml` file:
```bash
conda env create -f environment.yml
```
If the environment already exists, update it with:
```bash
conda env update --name myenv --file environment.yml --prune
```

## Running the Evaluation
After setting up the environment, run the evaluation with:

```bash
python3 script/compute.py --data_path  /home/user/session1.log --out_path . --dir_name session1
```
The `--data_path`, `--out_path` and `--dir_name` arguments function as previously described.
The results are exported to the **/output** directory within the specified path under the given directory name.
