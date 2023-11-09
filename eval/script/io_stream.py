import csv
import os


def parse_log_data(data_path):
    with open(data_path, 'r') as log_file:
        raw_data = log_file.read()
    entries = raw_data.strip().strip('\n').split('\n\n')
    parsed_data = []

    for ent in entries:
        lines = ent.split('\n')
        timestamp = lines[0]
        name = lines[1].split(',')[1]
        points = []
        for l in lines[1:]:
            coords = l.split(',')
            if len(coords) > 2:
                vals = float(coords[3]), float(coords[4]), float(coords[5])
                points.append(vals)
        #{toolhead: [[toolbase, tooltip, holebase holeend],timestamp]}
        parsed_data.append({name: [points, timestamp]})
    return parsed_data


def export_to_csv(data, out_path):
    csv_filename = os.path.join(out_path, "results.csv")
    headers = ["Name", "Position_Error", "Position_Diff_Vector" "Rotation_Error"]

    with open(csv_filename, 'w', newline='') as csvfile:
        csvw = csv.writer(csvfile)
        csvw.writerow(headers)
        for event in data:
            row = [
                list(event)[0],
                list(event.values())[0][0],
                list(event.values())[0][1],
                list(event.values())[0][2]
            ]
            csvw.writerow(row)


def export_stats_to_csv(data, out_path):
    stat_types = {f"{key}.csv": key for key in next(iter(data.values())).keys()}
    file_paths = {file_name: os.path.join(out_path, file_name) for file_name in stat_types.keys()}

    for file_name, k in stat_types.items():
        with open(file_paths[file_name], 'w', newline='') as csvfile:
            if k in next(iter(data.values())):
                subkeys = data[next(iter(data))][k].keys()
                header = ['Name'] + [str(subkey).capitalize() for subkey in subkeys]
                csvw = csv.DictWriter(csvfile, fieldnames=header)
                csvw.writeheader()
            for tool_name, tool_data in data.items():
                if k in tool_data:
                    stats = tool_data[k]
                    row_data = {'Name': tool_name}
                    row_data.update({str(k).capitalize(): v for k, v in stats.items()})
                    csvw.writerow(row_data)
