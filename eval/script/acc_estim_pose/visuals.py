import matplotlib.pyplot as plt
import pandas as pd
import os
from tabulate import tabulate
from util import create_out_directory

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
    #TODO export that file
    print(latex_table)


def draw_boxplot_from_csv(csv_file, _name):
    # TODO: add the values to the plot, do the green color of the mean, y label
    # Read data from the CSV file

    data = pd.read_csv(csv_file)

    # Extract the columns you want to plot
    metrics_info = data[['Max', 'Min', 'Mean', 'Median', 'Std', 'First Quartile', 'Fourth Quartile']]

    transposed_data = metrics_info.T
    plt.figure(figsize=(10, 6))

    plt.boxplot(transposed_data.values, labels=data['Name'], notch=False, sym='+', vert=True, whis=1.5)

    plt.xlabel("Tools' names")
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
                            required=False)
    parser.add_argument('--rot_diff_csv', type=str, help='The path to the csv file with rotation difference',
                            required=False)
    parser.add_argument('--out_path', type=str, help='The save path for plots', required=False)
    args = parser.parse_args()
    save_path = create_out_directory(args.out_path)
    #TODO: add the logging
    print("Save directory: ", save_path)
    if args.rot_diff_csv:
        print("Plotting rotation difference")
        plot_rotation_accuracy(args.rot_diff_csv, save_path)
        draw_latex_table(args.rot_diff_csv)
    else:
        print("No rotation difference csv file provided")
    if args.pos_diff_csv:
        print("Plotting position difference")
        plot_position_accuracy(args.pos_diff_csv, save_path)
        draw_latex_table(args.pos_diff_csv)
    else:
        print("No position difference csv file provided")
