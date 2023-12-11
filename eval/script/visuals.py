import matplotlib.pyplot as plt
import pandas as pd
import os
from tabulate import tabulate
import glob
import numpy as np
from matplotlib.lines import Line2D

CYBERGREEN = '#2DDE98'

def export_box_plot(csv_path: str, out_path: str) -> None:
    """
        Exports the boxplot of the statistics (mean, median, std, min, max, q1, q3)

        Args:
            csv_path (str): The path to the csv files
        Returns:
            None
    """
    csv_files = glob.glob(os.path.join(csv_path, '*_error.csv'))
    for csv_file in csv_files:
        save_plot = os.path.join(out_path, "boxplot_{}_graph.png".format(csv_file.split('/')[-1].split('.')[0]))
        box_plot = draw_boxplot_from_csv(csv_file=csv_file, _name="{}".format(csv_file.split('/')[-1].split('.')[0]))
        box_plot.savefig(save_plot)
        print(f"\033[90m[INFO]: Boxplot is exported to {save_plot}\033[0m")



def export_latex_table(csv_path: str, out_path: str) -> None:
    """
        Exports the latex table of the statistics (mean, median, std, min, max, q1, q3)

        Args:
            csv_path (str): The path to the csv files
        Returns:
            None
    """
    csv_files = glob.glob(os.path.join(csv_path, '*_error.csv'))
    for csv_file in csv_files:
        save_path = os.path.join(out_path, 'latex_{}.tex'.format(csv_file.split('/')[-1].split('.')[0]))
        data = pd.read_csv(csv_file)
        data_dict = data.to_dict(orient='records')
        latex_table = tabulate(data_dict, headers="keys", tablefmt="latex")
        with open(save_path, 'w') as file:
            file.write(latex_table)
        print(f"\033[90m[INFO]: Latex table is exported to {save_path}\033[0m")


def draw_boxplot_from_csv(csv_file: str, _name: str) -> plt.figure:
    """
        Draws the boxplot of the statistics (mean, median, std, min, max, q1, q3)

        Args:
            csv_path (str): The path to the csv files
            _name (str): The name of the plot
        Returns:
            plt.figure: The boxplot
    """
    data = pd.read_csv(csv_file)

    _name = ' '.join([word.capitalize() for word in _name.split('_')])
    metrics_info = data[['Max', 'Min', 'Mean', 'Median', 'Std', 'Q1', 'Q3']]
    transposed_data = metrics_info.T

    fig, ax = plt.subplots(figsize=(10., 6.))
    ax.set_xlabel("Tools' names")
    ax.spines['right'].set_visible(False)
    ax.spines['top'].set_visible(False)
    ax.spines['left'].set_visible(True)
    ax.spines['bottom'].set_visible(True)

    if _name.split(' ')[0] == 'Rotation':
        ax.set_ylabel(f"Angular Error [deg]")
    else:
        ax.set_ylabel(f"Distance Error [mm]")

    boxplot_elements = plt.boxplot(transposed_data.values, labels=data['Name'], notch=False, sym='+', vert=True,
                                   whis=1.5,  positions=None, widths=None, bootstrap=None, usermedians=None,
                                   conf_intervals=None, showfliers=True, showcaps=True, showbox=True)
    data_range = max(metrics_info.max()) - min(metrics_info.min())
    step = data_range * 0.025

    for whisker in boxplot_elements['whiskers']:
        whisker.set(color='black', linewidth=1)
    for cap in boxplot_elements['caps']:
        cap.set(color='black', linewidth=2)
    for median in boxplot_elements['medians']:
        median.set(linewidth=0)
    for flier in boxplot_elements['fliers']:
        flier.set(marker='+', color="black", alpha=0.5)
    for i, box in enumerate(boxplot_elements['boxes']):
        x = box.get_xdata()[0]

        max_val = metrics_info['Max'].iloc[i]
        min_val = metrics_info['Min'].iloc[i]
        mean_val = metrics_info['Mean'].iloc[i]
        q1_val = metrics_info['Q1'].iloc[i]
        q3_val = metrics_info['Q3'].iloc[i]

        positions = np.linspace(start=max_val + step, stop=max_val + 6 * step, num=6)

        plt.text(x, positions[0], f"Mx={max_val:.2f}", ha='left', va='bottom', fontsize='x-small')
        plt.text(x, positions[1], f"mn={mean_val:.2f}", ha='left', va='bottom', fontsize='x-small')
        plt.text(x, positions[2], f"q3={q3_val:.2f}", ha='left', va='bottom', fontsize='x-small')
        plt.text(x, positions[3], f"q1={q1_val:.2f}", ha='left', va='bottom', fontsize='x-small')
        plt.text(x, positions[4], f"M={min_val:.2f}", ha='left', va='bottom', fontsize='x-small')

        plt.plot([i + 1 - 0.25, i + 1 + 0.25], [mean_val, mean_val], color=CYBERGREEN, linewidth=2)

    mean_legend_entry = Line2D([0], [0], color=CYBERGREEN, linewidth=2, label='Mean')
    plt.legend(handles=[mean_legend_entry], loc='upper right', bbox_to_anchor=(1.1, 1.1), fontsize='x-small')

    num_tools = len(data['Name'])
    ax.set_xticks(np.arange(num_tools))
    plt.xticks(rotation=15)

    plt.tight_layout()
    return plt
