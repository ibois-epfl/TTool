import numpy as np
import matplotlib.pyplot as plt
import pandas as pd
import os
from tabulate import tabulate

CYBERGREEN = '#2DDE98'
CYBERGRAPE = '#6F2DBD'
CYBERBLUE = '#2D9CDB'
CYBERORANGE = '#F2994A'

def plot_rotation_accuracy(csv_file, out_path):
    save_plot = os.path.join(out_path, "rotation_accuracy.png")
    plot_rotation = draw_boxplot_from_csv(csv_file=csv_file, _name="Rotation")
    plot_rotation.savefig(save_plot)


def plot_position_accuracy(csv_file, out_path):
    save_plot = os.path.join(out_path, "position_accuracy.png")
    plot_rotation = draw_boxplot_from_csv(csv_file=csv_file, _name="Position")
    plot_rotation.savefig(save_plot)


def draw_latex_table(csv_file):
    data = pd.read_csv(csv_file)
    data_dict = data.to_dict(orient='records')
    latex_table = tabulate(data_dict, headers="keys", tablefmt="simple")

def draw_boxplot_from_csv(csv_file, _name):
    # Read data from the CSV file
    data = pd.read_csv(csv_file)

    # Extract the columns you want to plot
    metrics_info = data[['Max', 'Min', 'Mean', 'Median', 'Std', 'First Quartile', 'Fourth Quartile']]

    transposed_data = metrics_info.T
    plt.figure(figsize=(10, 6))

    plt.boxplot(transposed_data.values, labels=data['Name'], notch=False, sym='+', vert=True, whis=1.5)

    plt.xlabel("Tools' names (nbr of operations)")
    plt.ylabel("Error")
    plt.title("{} error".format(_name))

    # Rotate the x-axis labels for better readability
    plt.xticks(rotation=10)
    plt.tight_layout()
    return plt


if __name__ == '__main__':
    import argparse

    parser = argparse.ArgumentParser(description='Visualize pose estimation accuracy')
    parser.add_argument('--pos_diff_csv', type=str, help='The path to the csv file with position difference',
                            required=True)
    parser.add_argument('--rot_diff_csv', type=str, help='The path to the csv file with rotation difference',
                            required=True)
    parser.add_argument('--out_path', type=str, help='The save path for plots', required=True)
    args = parser.parse_args()
    plot_rotation_accuracy(args.rot_diff_csv, args.out_path)
    plot_position_accuracy(args.pos_diff_csv, args.out_path)
    draw_latex_table(args.pos_diff_csv)
    draw_latex_table(args.rot_diff_csv)
