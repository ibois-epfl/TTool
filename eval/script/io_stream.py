import csv
import os


def parse_log_data(data_path: str) -> list[dict[str, list[list[float]], str]]:
    """
        Processes the log file containing coordinates from AC,
        converting it into a more usable format for further processing

        Args:
            data_path (str): The path of log file with coordinates from AC

        Returns:
            dict[str, str, str]: the parsed data
    """
    ac_scale_factor = 50
    with open(data_path, 'r') as log_file:
        raw_data = log_file.read()
    entries = raw_data.strip().strip('\n').split('\n\n')
    parsed_data = list()

    for ent in entries:
        lines = ent.split('\n')
        timestamp = lines[0]
        name = lines[1].split(',')[1]
        points = list()
        for l in lines[1:]:
            coords = l.split(',')
            if len(coords) > 2:
                vals = float(coords[3])/ac_scale_factor, float(coords[4])/ac_scale_factor, float(coords[5])/ac_scale_factor
                points.append(vals)
        #{toolhead: [[toolbase, tooltip, holebase holeend],timestamp]}
        parsed_data.append({name: [points, timestamp]})
    return parsed_data


def export_to_csv(data: list, out_path: str) -> None:
    """
        Exports the computed results to a csv file

        Args:
            data (list): The computed results
        Returns:
            None
    """
    csv_filename = os.path.join(out_path, "results.csv")
    headers = ["Name", "Mean_Pos_Error", "Base_Pos_Error" "Tip_Pos_Error", "Rot_Error"]

    with open(csv_filename, 'w', newline='') as csvfile:
        csvw = csv.writer(csvfile)
        csvw.writerow(headers)
        for event in data:
            row = [
                list(event)[0],
                list(event.values())[0][0],
                list(event.values())[0][1],
                list(event.values())[0][2],
                list(event.values())[0][3]
            ]
            csvw.writerow(row)
    print(f"\033[90m[INFO]: Overall results exported to {csv_filename}\033[0m")

def export_stats_to_csv(data: dict, out_path: str) -> None:
    """
        Exports the statistics of the computed results to a csv file

        Args:
            data (dict): The computed statistics
        Returns:
            None
    """
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

        print(f"\033[90m[INFO]: Statistics results exported to {file_paths[file_name]}\033[0m")
