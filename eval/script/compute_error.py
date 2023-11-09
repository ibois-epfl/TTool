import numpy as np
from collections import defaultdict


def v_compute(p1, p2):
    return p2 - p1


def a_difference(vec1, vec2):
    dot_prod = np.dot(vec1, vec2)
    m_A = np.linalg.norm(vec1)
    m_B = np.linalg.norm(vec2)
    cos_theta = dot_prod / (m_A * m_B)
    cos_theta = np.clip(cos_theta, -1, 1)
    theta = np.arccos(cos_theta)
    theta_degrees = np.degrees(theta)
    return theta_degrees


def euclidean_dist_p(p1, p2):
    return np.sqrt(np.sum((p1 - p2) ** 2))


def mean_dist(n1, n2):
    return (n1 + n2) / 2


def compute_pose_estim_error(data):

    results = list()
    for entry in data:
        # {toolhead: [[toolbase, tooltip, holebase holeend],timestamp]}
        coords = list(entry.values())[0][0]
        base_er = euclidean_dist_p(np.array(coords[0]), np.array(coords[2]))
        tip_err = euclidean_dist_p(np.array(coords[1]), np.array(coords[3]))
        mean_err = mean_dist(base_er, tip_err)

        v_tool = v_compute(np.array(coords[0]), np.array(coords[1]))  # the vector between toolbase and tooltip
        v_hole = v_compute(np.array(coords[2]), np.array(coords[3]))  # the vector between holebase and holeend

        if v_tool is not None and v_hole is not None:
            rot_err = a_difference(v_tool, v_hole)  # the angle between two vectors

            results.append({list(entry)[0]: [mean_err, base_er, tip_err, rot_err]})
    return results


def stats(data):
    return {
        "max": np.max(data),
        "min": np.min(data),
        "mean": np.mean(data),
        "median": np.median(data),
        "std": np.std(data),
        "q1": np.percentile(data, 25),
        "q3": np.percentile(data, 75),
    }


def compute_stats(data):
    o_data = defaultdict(lambda: defaultdict(list))

    for ent in data:
        for k, v in ent.items():
            o_data[k]['mean_pos_err'].append(v[0])
            o_data[k]['base_pos_err'].append(v[1])
            o_data[k]['tip_pos_err'].append(v[2])
            o_data[k]['rot_err'].append(v[3])

    o_stats = {}
    for name, value_dict in o_data.items():
        o_stats[name] = {
            'mean_pos_err': stats(value_dict['mean_pos_err']),
            'base_pos_err': stats(value_dict['base_pos_err']),
            'tip_pos_err': stats(value_dict['tip_pos_err']),
            'rot_err': stats(value_dict['rot_err']),
        }

    return o_stats


