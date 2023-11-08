import numpy as np
import csv
import os

def parse_log_data(data_path):
    with open(data_path, 'r') as log_file:
        raw_data = log_file.read()
    entries = raw_data.strip().strip('\n').split('\n\n')
    parsed_data = []

    for entry in entries:
        lines = entry.split('\n')
        timestamp = lines[0]

        name = lines[1].split(',')[1]
        points = []
        for line in lines[1:]:
            coords = line.split(',')
            if len(coords) > 2:
                values = float(coords[3]), float(coords[4]), float(coords[5])
                points.append(values)
        #{toolhead: [[toolbase, tooltip, holebase holeend],timestamp]}
        parsed_data.append({name: [points, timestamp]})
    return parsed_data


def dump_results_to_csv(results, filename='results.csv'):

    csv_filename = os.path.join(filename, "results.csv")
    headers = ["Name", "Position_Error", "Position_Diff_Vector" "Rotation_Error"]

    with open(csv_filename, 'w', newline='') as csvfile:
        csvwriter = csv.writer(csvfile)
        csvwriter.writerow(headers)
        for event in results:
            row = [
                list(event)[0],
                list(event.values())[0][0],
                list(event.values())[0][1],
                list(event.values())[0][2]
            ]
            csvwriter.writerow(row)


def flatten_stats(name, stat_type, stats):
    return [name + '_' + stat_type] + [stats[key] for key in ['max', 'min', 'mean', 'median', 'std', 'q1', 'q4']]


def dump_stats_to_csv(stats_results, path):
    o_files = ['p_error.csv', 'diff_pos_X.csv', 'diff_pos_Y.csv', 'diff_pos_Z.csv', 'r_error.csv']
    header = ["Name", "Max", "Min", "Mean", "Median", "Std", "Q1", "Q4"]
    for o in o_files:
        csv_f = os.path.join(path, o)
        with open(csv_f, 'w', newline='') as csvfile:
            writer = csv.DictWriter(csvfile, fieldnames=header)

            # Write the header row
            writer.writeheader()

            # Write the data
            for tool_name, tool_data in stats_results.items():
                for stat_type, stats in tool_data.items():
                    row_data = {
                        'Name': tool_name,
                        'Max': stats['max'],
                        'Min': stats['min'],
                        'Mean': stats['mean'],
                        'Median': stats['median'],
                        'Std': stats['std'],
                        'Q1': stats['q1'],
                        'Q4': stats['q4']
                    }
                    writer.writerow(row_data)
