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
            out_path (str): The path to the output directory
        Returns:
            None
    """
    csv_files = glob.glob(os.path.join(csv_path, '*_error.csv'))
    for csv_file in csv_files:
        save_plot = os.path.join(out_path, "boxplot_{}_graph.png".format(csv_file.split('/')[-1].split('.')[0]))
        box_plot = draw_boxplot_from_csv(csv_file=csv_file, _name="{}".format(csv_file.split('/')[-1].split('.')[0]))
        box_plot.savefig(save_plot)
        box_plot.close()
        print(f"\033[90m[INFO]: Boxplot is exported to {save_plot}\033[0m")


def transform_name(name: str) -> str:
    """
        Transforms the name of the tool to a latex format

        Args:
            name (str): The name of the tool
        Returns:
            str: The name of the tool in latex format
    """
    name_map = {
        "auger_drill_bit_20_235": "drill auger $\\varnothing$20",
        "spade_drill_bit_25_150": "drill spade $\\varnothing$25",
        "auger_drill_bit_35_235": "drill auger $\\varnothing$35",
        "brad_point_drill_bit_20_150": "drill brad point $\\varnothing$20",
        "twist_drill_bit_32_165": "drill twist $\\varnothing$32",
        "self_feeding_bit_50_90": "drill self feeding $\\varnothing$50",
        "circular_saw_blade_makita_190": "circular sawblade",
        "chain_swordsaw_blade_200": "sword sawblade",
        "chain_saw_blade_f_250": "chain sawblade"
    }
    return name_map.get(name, name)


def export_latex_table(csv_path: str, out_path: str, num_of_operations: dict) -> None:
    """
        Exports the latex table of the statistics of position and rotation errors

        Args:
            csv_path (str): The path to the csv files
            out_path (str): The path to the output directory
            num_of_operations (dict): The number of operations for each tool
        Returns:
            None
    """
    csv_files = glob.glob(os.path.join(csv_path, '*_error.csv'))
    combined_df = pd.DataFrame()
    new_df = pd.DataFrame()
    for csv_file in csv_files:
        latex_header_l = csv_file.split('/')[-1].split('.')[0].split('_')[:-1]
        latex_header = "_".join(latex_header_l)
        df = pd.read_csv(csv_file)
        df['Number of operations (-)'] = df['Name'].map(num_of_operations)
        df['Name'] = df['Name'].apply(transform_name)
        if latex_header == "mean_position":
            df['Mean Position Error * (mm)'] = df['Mean'].round(2).astype(str) + ' ± ' + df['Std'].round(2).astype(str)
            new_df = df[['Name', 'Number of operations (-)', 'Mean Position Error * (mm)']].copy()
            new_df.rename(columns={'Name': 'Tool Name (-)'}, inplace=True)
        if latex_header == "rotation":
            df['Mean Rotation Error * (°)'] = df['Mean'].round(2).astype(str) + ' ± ' + df['Std'].round(2).astype(str)
            new_df = df[['Name', 'Number of operations (-)', 'Mean Rotation Error * (°)']].copy()
            new_df.rename(columns={'Name': 'Tool Name (-)'}, inplace=True)
        combined_df = pd.concat([combined_df, new_df], axis=1)

    combined_df = combined_df.loc[:, ~combined_df.columns.duplicated()]
    save_path = os.path.join(out_path, 'latex_table.tex')
    num_cols = len(combined_df.columns)
    column_format = '|'.join(['c'] * num_cols)
    latex_table_content = combined_df.to_latex(index=False, escape=False, column_format=column_format, header=True,
                                               longtable=False)
    header_latex_content = latex_table_content.split("\\midrule")[0]
    mid_table_content = latex_table_content.split("\\midrule")[1]
    latex_table_content = "\\hline\n" + header_latex_content + "\\hline" + mid_table_content

    with open(save_path, 'w') as file:
        file.write(latex_table_content)
        print(f"\033[90m[INFO]: Latex table is exported to {save_path}\033[0m")


def draw_boxplot_from_csv(csv_file: str, _name: str) -> plt.figure:
    """
        Draws the boxplot of the statistics (mean, median, std, min, max, q1, q3)

        Args:
            csv_file (str): The csv file with the statistics
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
    ax.set_xticks(np.arange(0, num_tools + 1))

    plt.xticks(rotation=15)

    plt.tight_layout()
    return plt


def draw_progression_graph(csv_path: str, out_path: str) -> plt.figure:
    """
        Draws the progression graph of the results with respect to the number of operations

        Args:
            csv_path (str): The path to the csv files
            out_path (str): The path to the output directory
        Returns:
            plt.figure: The progression graph
    """
    csv_file = os.path.join(csv_path, 'results.csv')
    df = pd.read_csv(csv_file)

    colors = plt.cm.tab20(np.linspace(0, 1, df['Name'].nunique()))

    columns = df.columns.drop('Name')
    for column in columns:
        data = df.groupby('Name')[column].apply(list).reset_index()
        max_length = max(data[column].apply(len))
        fig, ax = plt.subplots(figsize=(8, 4))

        for index, row in data.iterrows():
            tool_name = row['Name']
            values = row[column]
            tool_color = colors[index % len(colors)]
            categories = np.arange(1, len(values)+1)
            ax.plot(categories, values, color=tool_color, linewidth=2, label=tool_name)
            ax.scatter(categories, values, color=tool_color, s=1)

        if column.split('_')[0] == 'Rotation':
            y_label = 'Angular Error [deg]'
        else:
            y_label = 'Distance Error [mm]'
        ax.set_ylabel(y_label)
        ax.set_xlabel('Number of operations')
        ax.set_xticks(np.arange(1, max_length + 1))

        ax.spines['right'].set_visible(False)
        ax.spines['top'].set_visible(False)

        ax.yaxis.grid(False)

        ax.legend(loc='upper left', bbox_to_anchor=(1, 1))

        plt.tight_layout()
        plt.savefig(os.path.join(out_path, f'progression_{column.lower()}_graph.png'))
        plt.close(fig)
