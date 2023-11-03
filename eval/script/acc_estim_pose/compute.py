import numpy as np
from io_stream import parse_log_data, dump_accuracy_to_csv, dump_stats_to_csv
from collections import defaultdict


def compute_vector(point1, point2):
    return point2 - point1


def vector_difference(vec1, vec2):
    return vec1 - vec2


def angle_differences(vec1, vec2):
    dot_product = np.dot(vec1, vec2)
    magnitude_A = np.linalg.norm(vec1)
    magnitude_B = np.linalg.norm(vec2)
    cosine_theta = dot_product / (magnitude_A * magnitude_B)

    # Clamp the value between -1 and 1 to avoid numerical precision issues
    cosine_theta = np.clip(cosine_theta, -1, 1)

    theta = np.arccos(cosine_theta)

    # Convert the angle from radians to degrees
    theta_degrees = np.degrees(theta)

    return theta_degrees


def vector_magnitude(vector):
    return np.linalg.norm(vector)


def compute_pose_estimation_accuracy(log_data):
    events = []
    for entry in log_data:
        points = entry["points"]

        tool_vector = None
        hole_vector = None

        if "ToolbaseGO" in points and "TooltipGO" in points:
            tool_vector = compute_vector(points["ToolbaseGO"], points["TooltipGO"])
        elif "ChainBaseGO" in points and "NormStartGO" in points:
            tool_vector = compute_vector(points["ChainBaseGO"], points["NormStartGO"])
        elif "CenterGO" in points and "NormEndGO" in points:
            tool_vector = compute_vector(points["CenterGO"], points["NormEndGO"])
        if "StartPoint" in points and "EndPoint" in points:
            hole_vector = compute_vector(points["StartPoint"], points["EndPoint"])

        if tool_vector is not None and hole_vector is not None:
            diff_vector = vector_difference(tool_vector, hole_vector)
            diff_position = vector_magnitude(diff_vector)
            diff_rotation = angle_differences(tool_vector, hole_vector)

            events.append({
                "name": entry["name"],
                "position_difference": diff_position,
                "rotation_difference": diff_rotation

            })
    return events


def stats(data):
    print(data)
    return {'max': np.max(data),
            'min': np.min(data),
            'mean': np.mean(data),
            'median': np.median(data),
            'std': np.std(data),
            'first_quartile': np.percentile(data, 25),
            'fourth_quartile': np.percentile(data, 75)
            }


def compute_stats(data, stats_diff_type):
    # Group data by name
    global position_diff_values
    grouped_data = defaultdict(list)
    for item in data:
        name = item['name']
        grouped_data[name].append(item)

    # Compute statistics for each group
    result = []

    for name, group in grouped_data.items():
        position_diff_values = [item[stats_diff_type] for item in group]

        result.append({'name': name,
                       stats_diff_type: stats(position_diff_values)})

    return result


if __name__ == '__main__':
    import argparse
    parser = argparse.ArgumentParser(description='Compute pose estimation accuracy')
    parser.add_argument('--data_path', type=str, help='The path of log file with coordinates from AC', required=True)
    parser.add_argument('--out_path', type=str, help='The save path for resulted csv', required=True)
    args = parser.parse_args()

    acc_results = compute_pose_estimation_accuracy(parse_log_data(args.data_path))
    dump_accuracy_to_csv(acc_results, args.out_path)
    pos_stats_results = compute_stats(acc_results, stats_diff_type='position_difference')
    rot_stats_results = compute_stats(acc_results, stats_diff_type='rotation_difference')

    dump_stats_to_csv(pos_stats_results, args.out_path, filename='pos_diff.csv', entry_name='position_difference')
    dump_stats_to_csv(rot_stats_results, args.out_path, filename='rot_diff.csv', entry_name='rotation_difference')


