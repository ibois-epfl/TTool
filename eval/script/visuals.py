import matplotlib.pyplot as plt
import pandas as pd
import os
from tabulate import tabulate
import glob
import numpy as np


def export_box_plot(csv_path, out_path):
    csv_files = glob.glob(os.path.join(csv_path, '*_err.csv'))
    for csv_file in csv_files:
        save_plot = os.path.join(out_path, "{}.png".format(csv_file.split('/')[-1].split('.')[0]))
        box_plot = draw_boxplot_from_csv(csv_file=csv_file, _name="{}".format(csv_file.split('/')[-1].split('.')[0]))
        box_plot.savefig(save_plot)


def export_latex_table(csv_path, out_path):
    csv_files = glob.glob(os.path.join(csv_path, '*_err.csv'))
    for csv_file in csv_files:
        save_path = os.path.join(out_path, '{}.tex'.format(csv_file.split('/')[-1].split('.')[0]))
        data = pd.read_csv(csv_file)
        data_dict = data.to_dict(orient='records')
        latex_table = tabulate(data_dict, headers="keys", tablefmt="latex")
        with open(save_path, 'w') as file:
            file.write(latex_table)


def draw_boxplot_from_csv(csv_file, _name):
    data = pd.read_csv(csv_file)
    metrics_info = data[['Max', 'Min', 'Mean', 'Median', 'Std', 'Q1', 'Q3']]

    transposed_data = metrics_info.T
    plt.figure(figsize=(10, 6))

    boxplot_elements = plt.boxplot(transposed_data.values, labels=data['Name'], notch=False, sym='+', vert=True,
                                   whis=1.5)
    data_range = max(metrics_info.max()) - min(metrics_info.min())
    step = data_range * 0.03

    for i, (box, median_line) in enumerate(zip(boxplot_elements['boxes'], boxplot_elements['medians'])):
        x = box.get_xdata()[0]
        top_of_box = box.get_ydata()[2]

        max_val = metrics_info['Max'].iloc[i]
        min_val = metrics_info['Min'].iloc[i]
        mean_val = metrics_info['Mean'].iloc[i]
        q1_val = metrics_info['Q1'].iloc[i]
        q3_val = metrics_info['Q3'].iloc[i]

        positions = np.linspace(start=top_of_box + step, stop=top_of_box + 6 * step, num=6)

        plt.text(x, positions[0], f"Mx={max_val:.2f}", ha='center', va='bottom', fontsize='x-small')
        plt.text(x, positions[2], f"q3={q3_val:.2f}", ha='center', va='bottom', fontsize='x-small')
        plt.text(x, positions[1], f"mn={mean_val:.2f}", ha='center', va='bottom', fontsize='x-small')
        plt.text(x, positions[3], f"q1={q1_val:.2f}", ha='center', va='bottom', fontsize='x-small')
        plt.text(x, positions[4], f"M={min_val:.2f}", ha='center', va='bottom', fontsize='x-small')

    means = metrics_info['Mean']
    for i, mean in enumerate(means):
        plt.plot([i + 1 - 0.25, i + 1 + 0.25], [mean, mean], color='green', linewidth=2)

    plt.xlabel("Tools' names")
    plt.ylabel("{}".format(_name.capitalize()))
    plt.title("{}".format(_name.capitalize()))

    plt.xticks(rotation=10)
    plt.tight_layout()
    return plt
