import numpy as np
from io_stream import parse_log_data, write_results_to_csv

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


def compute_pose_estimation_accuracy(log_data):
    events = []
    for entry in log_data:
        points = entry["points"]

        # Compute vectors for tools and holes
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
            # Compute the difference in the vectors
            diff_vector = vector_difference(tool_vector, hole_vector)
            diff_angle = angle_differences(tool_vector, hole_vector)

            # Store in events
            events.append({
                "timestamp": entry["timestamp"],
                "name": entry["name"],
                "tool_vector": tool_vector,
                "hole_vector": hole_vector,
                "position_difference": diff_vector,
                "rotation_difference": diff_angle

            })
    return events


def compute_min(data):
    return np.min(data)


def compute_max(data):
    return np.max(data)


def compute_mean(data):
    return np.mean(data)


def compute_median(data):
    return np.median(data)


def compute_std(data):
    return np.std(data)


def compute_percentile(data, percentile):
    return np.percentile(data, percentile)


if __name__ == '__main__':
    import argparse
    parser = argparse.ArgumentParser(description='Compute pose estimation accuracy')
    parser.add_argument('--data_path', type=str, help='The path of log file with coordinates from AC', required=True)
    parser.add_argument('--out_path', type=str, help='The save path for resulted csv', required=True)
    args = parser.parse_args()

    results = compute_pose_estimation_accuracy(parse_log_data(args.data_path))
    write_results_to_csv(results, args.out_path)
