import numpy as np
from collections import defaultdict


def v_compute(p1: np.array(float), p2: np.array(float)) -> np.array(float):
    """
        Compute the vector between two points in 3D space
        (in this case, the base and the tip of the tool or the base and the tip of the hole, so then we can calculate
        the angle between two vectors)

        Args:
            p1 (np.array(float)): The first point
            p2 (np.array(float)): The second point
        Returns:
            np.array(float): The vector between the two points (the vector of the tool or the vector of the hole)
    """
    return p2 - p1


def a_difference(vec1: np.array(float), vec2: np.array(float)) -> np.array(float):
    """
        Compute the angle between two vectors in 3D space

        Args:
            vec1 (np.array(float)): The first vector
            vec2 (np.array(float)): The second vector
        Returns:
            np.array(float): The angle between the two vectors, thus the rotation error
    """
    dot_prod = np.dot(vec1, vec2)
    m_A = np.linalg.norm(vec1)
    m_B = np.linalg.norm(vec2)
    cos_theta = dot_prod / (m_A * m_B)
    cos_theta = np.clip(cos_theta, -1, 1)
    theta = np.arccos(cos_theta)
    theta_degrees = np.degrees(theta)
    return theta_degrees


def euclidean_dist_p(p1: np.array(float), p2: np.array(float)) -> np.array(float):
    """
        Compute the euclidean distance between two points in 3D space
        (in this case, the base of the toolhead and the hole and the tip of the toolhead and the hole)

        Args:
            p1 (np.array(float)): The first point
            p2 (np.array(float)): The second point
        Returns:
            np.array(float): The euclidean distance between the two points in 3D space, thus the position error
    """
    return np.sqrt(np.sum((p1 - p2) ** 2))


def mean_dist(n1: np.array(float), n2: np.array(float)) -> np.array(float):
    """
        Compute the mean distance between two distances
        Args:
            n1 (np.array(float)): The first distance
            n2 (np.array(float)): The second distance
        Returns:
            np.array(float): The mean distance between the two distances
    """
    return (n1 + n2) / 2


def compute_pose_error(data: list) -> list:
    """
        Compute the position and rotation errors between the toolhead and the hole

        Args:
            data (list): The parsed data
        Returns:
            list: A list containing the computed error metrics, including:
                - the computed position error of the base coordinates of the toolhead and the hole
                - the computed position error of the tip coordinates of the toolhead and the hole
                - the computed position error of the mean of the base and tip coordinates of the toolhead and the hole
                - the computed rotation error between the vectors of the toolhead and the hole
    """
    scale_f = 50
    results = list()
    for entry in data:
        # {toolhead: [[toolbase, tooltip, holebase holeend],timestamp]}
        coords = list(entry.values())[0][0]
        base_err: np.array(float) = euclidean_dist_p(np.array(coords[0]), np.array(coords[2]))
        tip_err: np.array(float) = euclidean_dist_p(np.array(coords[1]), np.array(coords[3]))
        mean_err: np.array(float) = mean_dist(base_err, tip_err)

        v_tool: np.array(float) = v_compute(np.array(coords[0]), np.array(coords[1]))  # the vector between toolbase and tooltip
        v_hole: np.array(float) = v_compute(np.array(coords[2]), np.array(coords[3]))  # the vector between holebase and holeend

        if v_tool is not None and v_hole is not None:
            rot_err: np.array(float) = a_difference(v_tool, v_hole)  # the angle between two vectors

            results.append({list(entry)[0]: [mean_err / scale_f, base_err/scale_f, tip_err/scale_f, rot_err]})
    return results


def stats(data: list) -> dict:
    """
        Helper function that computes the mean, median, std, q1, q3, max, min of the results:

        Args:
            data (list): The results of the computed position and rotation errors
        Returns:
            dict: The dictionary that contains mean, median, std, q1, q3, max, min of the results
    """
    return {
        "max": np.max(data),
        "min": np.min(data),
        "mean": np.mean(data),
        "median": np.median(data),
        "std": np.std(data),
        "q1": np.percentile(data, 25),
        "q3": np.percentile(data, 75),
    }


def compute_stats(data: list) -> dict:
    """
        Compute the statistics (mean, median, std, q1, q3, max, min) of the results:
        - the position error of the base coordinates of the toolhead and the hole
        - the position error of the tip coordinates of the toolhead and the hole
        - the position error of the mean of the base and tip coordinates of the toolhead and the hole
        - the rotation error between the vectors of the toolhead and the hole

        Args:
            data (list): The results of the computed position and rotation errors
        Returns:
            dict: The statistics of the computed results
    """
    o_data = defaultdict(lambda: defaultdict(list))

    for ent in data:
        for k, v in ent.items():
            o_data[k]['mean_position_error'].append(v[0])
            o_data[k]['base_position_error'].append(v[1])
            o_data[k]['tip_position_error'].append(v[2])
            o_data[k]['rotation_error'].append(v[3])

    o_stats = {}
    for name, value_dict in o_data.items():
        o_stats[name] = {
            'mean_position_error': stats(value_dict['mean_position_error']),
            'base_position_error': stats(value_dict['base_position_error']),
            'tip_position_error': stats(value_dict['tip_position_error']),
            'rotation_error': stats(value_dict['rotation_error']),
        }

    return o_stats


