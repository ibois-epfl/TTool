import numpy as np
import csv
import os

def parse_log_data(data_path):
    # Open and read the log file
    with open(data_path, 'r') as log_file:
        raw_data = log_file.read()
    entries = raw_data.strip().split('\n\n')  # Using strip() to handle trailing newlines
    parsed_data = []

    for entry in entries:
        lines = entry.split('\n')
        timestamp = lines[0]
        name = lines[1].split(',')[1]
        points = {parts[2]: np.array([float(parts[3]), float(parts[4]), float(parts[5])])
                  for parts in (line.split(',') for line in lines[1:]) if len(parts) > 2}

        parsed_data.append({"timestamp": timestamp, "name": name, "points": points})

    return parsed_data


def dump_accuracy_to_csv(results, filename='results.csv'):
    csv_filename = os.path.join(filename, "results.csv")
    headers = ["Name", "Position Difference", "Rotation Difference"]

    with open(csv_filename, 'w', newline='') as csvfile:
        csvwriter = csv.writer(csvfile)

        # Write the headers first
        csvwriter.writerow(headers)

        # Write data
        for event in results:
            row = [
                event["name"],
                event["position_difference"],
                event["rotation_difference"],
            ]
            csvwriter.writerow(row)


def dump_stats_to_csv(stats_results, path, filename, entry_name):
    csv_filename = os.path.join(path, filename)
    with open(csv_filename, 'w', newline='') as csvfile:
        csvwriter = csv.writer(csvfile)

        header = ["Name", "Max", "Min", "Mean", "Median", "Std", "First Quartile", "Fourth Quartile"]
        csvwriter.writerow(header)

        for entry in stats_results:
            name = entry['name']
            _entry = entry[entry_name]
            row = [name, _entry['max'], _entry['min'], _entry['mean'], _entry['median'],
                   _entry['std'], _entry['first_quartile'], _entry['fourth_quartile']]
            csvwriter.writerow(row)
