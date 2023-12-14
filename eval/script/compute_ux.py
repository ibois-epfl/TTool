import matplotlib.pyplot as plt
import numpy as np
import os

categories1 = ['Tool Selection', 'Input Pose', 'Final Pose']
values1 = [3.83, 4.5, 4.33]  # average ratings for each section

categories2 = ['auger_drill_bit_20_235', 'auger_drill_bit_34_235', 'spade_drill_bit_25_150',
               'brad_point_drill_bit_20_150', 'self_feeding_bit_50_90', 'twist_drill_bit_32_165',
               'sword_saw', 'circular_saw', 'chain_saw']
values2 = [3, 4, 4, 5, 5, 5, 5, 4, 5]  # ratings for each tool


def export_ux_graph(out_path: str) -> None:
    draw_spider_graph(categories1, values1, 'Progress Based Questions', out_path)
    draw_spider_graph(categories2, values2, 'Tool Based Questions', out_path)


def draw_spider_graph(categories, values, title, out_path: str):
    N = len(categories)
    angles = np.linspace(0, 2 * np.pi, N, endpoint=False).tolist()
    values += values[:1]
    angles += angles[:1]

    fig = plt.figure(figsize=(8, 4))
    ax = fig.add_subplot(111, polar=True)
    ax.set_theta_offset(np.pi / 2)
    ax.set_theta_direction(-1)

    ax.set_xticklabels(categories, color='grey', size=8, ha='center', va='center')

    plt.xticks(angles[:-1], categories, color='grey', size=8)
    ax.plot(angles, values)
    ax.fill(angles, values, 'b', alpha=0.1)
    ax.set_title(title, size=11, color='blue', y=1.1)
    print(title.lower())
    plt.tight_layout()
    plt.savefig(os.path.join(out_path, f'ux_{title.lower().replace(" ","_")}_graph.png'))
    plt.close()

