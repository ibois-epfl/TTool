import numpy as np
import csv
import os

def get_name(lines):
    tool_name = next((parts[1] for parts in (line.split(',') for line in lines) if parts[0] == "Toolhead"), None)
    hole_name = next((parts[1] for parts in (line.split(',') for line in lines) if parts[0] == "HOLE"), None)
    return f"{tool_name},{hole_name}" if tool_name and hole_name else None


def parse_log_data(data_path):
    # Open and read the log file
    with open(data_path, 'r') as log_file:
        raw_data = log_file.read()
    entries = raw_data.strip().split('\n\n')  # Using strip() to handle trailing newlines
    parsed_data = []

    for entry in entries:
        lines = entry.split('\n')
        timestamp = lines[0]
        combined_name = get_name(lines[1:])
        points = {parts[2]: np.array([float(parts[3]), float(parts[4]), float(parts[5])])
                  for parts in (line.split(',') for line in lines[1:]) if len(parts) > 2}

        parsed_data.append({"timestamp": timestamp, "name": combined_name, "points": points})

    return parsed_data


def write_results_to_csv(results, filename='results.csv'):
    csv_filename = os.path.join(filename, "results.csv")
    # Define the header for your CSV
    headers = ["Timestamp", "Name", "Tool Vector", "Hole Vector", "Position Difference Vector",
              "Rotation Difference Angle"]

    with open(csv_filename, 'w', newline='') as csvfile:
        csvwriter = csv.writer(csvfile)

        # Write the headers first
        csvwriter.writerow(headers)

        # Write data
        for event in results:
            row = [
                event["timestamp"],
                event["name"],
                ','.join(map(str, event["tool_vector"])),
                ','.join(map(str, event["hole_vector"])),
                event["position_difference"],
                event["rotation_difference"],
            ]
            csvwriter.writerow(row)
