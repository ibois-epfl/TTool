import numpy as np
from collections import defaultdict


def v_compute(p1, p2):
    return p2 - p1


def v_difference(vec1, vec2):
    return vec1 - vec2


def a_difference(vec1, vec2):
    dot_prod = np.dot(vec1, vec2)
    m_A = np.linalg.norm(vec1)
    m_B = np.linalg.norm(vec2)
    cos_theta = dot_prod / (m_A * m_B)
    cos_theta = np.clip(cos_theta, -1, 1)
    theta = np.arccos(cos_theta)
    theta_degrees = np.degrees(theta)
    return theta_degrees


def euclidean_dist(vec):
    return np.sqrt(np.sum(vec ** 2))


def v_mean(vec):
    return np.mean(vec, axis=0)


def compute_pose_estimation_accuracy(data):
    _v_tool = None
    _v_hole = None

    results = list()
    for entry in data:
        coords = list(entry.values())[0][0]
        _v_tool = v_compute(np.array(coords[0]), np.array(coords[1]))  # the vector between toolbase and tooltip
        _v_hole = v_compute(np.array(coords[2]), np.array(coords[3]))  # the vector between holebase and holeend

        if _v_tool is not None and _v_hole is not None:
            _v_diff_pos = v_difference(_v_tool, _v_hole)
            _v_mean = v_mean([_v_tool, _v_hole])  # the mean vector between two vectors
            _p_error = euclidean_dist(_v_diff_pos)  # the length of the line
            _r_error = a_difference(_v_tool, _v_hole)  # the angle between two vectors

            results.append({list(entry)[0]: [_p_error, [_v_diff_pos[0], _v_diff_pos[1], _v_diff_pos[2]], _r_error]})
    return results


def stats(data):
    # adapt this to the new data format with xyz
    return {
        "max": np.max(data),
        "min": np.min(data),
        "mean": np.mean(data),
        "median": np.median(data),
        "std": np.std(data),
        "q1": np.percentile(data, 25),
        "q4": np.percentile(data, 75),
    }


def compute_stats(data):
    o_data = defaultdict(lambda: defaultdict(list))

    for entry in data:
        for name, values in entry.items():
            o_data[name]['p_error'].append(values[0])
            o_data[name]['diff_pos_X'].append(values[1][0])
            o_data[name]['diff_pos_Y'].append(values[1][1])
            o_data[name]['diff_pos_Z'].append(values[1][2])
            o_data[name]['r_error'].append(values[2])

    o_stats = {}
    for name, value_dict in o_data.items():
        o_stats[name] = {
            'p_error_stats': stats(value_dict['p_error']),
            'diff_pos_X': stats(value_dict['diff_pos_X']),
            'diff_pos_Y': stats(value_dict['diff_pos_Y']),
            'diff_pos_Z': stats(value_dict['diff_pos_Z']),
            'r_error_stats': stats(value_dict['r_error'])
        }

    return o_stats


