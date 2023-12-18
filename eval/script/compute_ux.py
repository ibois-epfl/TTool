import matplotlib.pyplot as plt
import os
from visuals import draw_spider_graph

categories1 = ['Tool Selection', 'Input Pose', 'Final Pose']
values1 = [3.83, 4.5, 4.33]  # average ratings for each section

categories2 = ['auger_20_235', 'auger_34_235', 'spade_25_150',
               'brad_20_150', 'self_feed_50_90', 'twist_32_165',
               'sword_saw', 'circular_saw', 'chain_saw']
values2 = [3, 4, 4, 5, 5, 5, 5, 4, 5]  # ratings for each tool (not averaged)


def export_ux_graph(out_path: str) -> None:
    """
        Exports the UX graph to the output path.
        Note: The categories and values are hardcoded.

        Args:
            out_path (str): the output path
        Returns:
            None
    """
    graph_data = [
        {'title': 'Progress Based Questions', 'categories': categories1, 'values': values1},
        {'title': 'Tool Based Questions', 'categories': categories2, 'values': values2}
    ]

    for graph in graph_data:
        draw_spider_graph(graph['categories'], graph['values'], graph['title'])
        file_name = f"ux_{graph['title'].lower().replace(' ', '_')}_graph.png"
        plt.savefig(os.path.join(out_path, file_name))
        plt.close()

