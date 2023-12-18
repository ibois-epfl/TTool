
## TTool Evaluation Data Folder Arrangement
This readme provides a comprehensive guide to the folder arrangement for the data related to the Evaluation of the TTool. 
All data is stored on both the external hard drive and the IBOIS server.

## Storage Paths
- External hard drive: `/JG_Disk04/2023_ttool_eval_media`
- IBOIS server: `/media/18TB_ibois_stock/2023_ttool_eval_media`

## Folder arrangement details
The data is organized into the following folders:

- ```all_files``` 

This folder contains all the files exported during the TTool evaluation process. 
It contains **all the tools** and all **4 sessions (20 cycles)** of evaluation. 

1. **external_records**: Contains all photos and videos captured from an external perspective during the evaluation.
2. **logs**: Contains log files of **4 sessions (20 cycles)** exported from AC throughout the evaluation.
3. **recorder**: Contains screen recordings of **4 sessions (20 cycles)** captured from AC during the evaluation.
4. **screenshots**: Contains all screenshots from AC taken during the evaluation.

- ```correct_files``` 

This folder contains files exported during the evaluation that is used for the analysis. 
It does not contain the data of **self_feeding_bit_40_90** and it includes **4 sessions (18 cycles)** of evaluation. 

1. **external_records**: Contains all photos and videos captured from an external perspective during the evaluation.
2. **logs**: Contains log files of **4 sessions (18 cycles)** exported from AC throughout the evaluation.
It does not contain the log files of **self_feeding_bit_40_90**. 
3. **recorder**: Contains screen recordings of **4 sessions (18 cycles)** captured from AC during the evaluation.
It does not contain the screen recordings of **self_feeding_bit_40_90**.
4. **screenshots**: Contains all screenshots from AC taken during the evaluation.

- ```output``` 
This folder contains all the results of the evaluation. 
The folder is organized into the several subfolders: **all_sessions**, **session1**, **session2**, **session3**, **session4**.
- **all_sessions**: Contains the results of the evaluation of all sessions.
- **session1** - **session4**: Contains the results of the evaluation of each session.

Each of these folders contains the following subfolders: **boxplot**, **csv**, **latex**, **progression**, **ux**.
- **boxplot**: Contains the boxplot graphs of the evaluation results.
- **csv**: Contains the CSV files of the evaluation results.
- **latex**: Contains the LaTeX tables of the evaluation results.
- **progression**: Contains the progression graphs of the evaluation results.
- **ux**: Contains the UX graphs of the evaluation results.

