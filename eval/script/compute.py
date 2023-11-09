from io_stream import parse_log_data, export_to_csv, export_stats_to_csv
from util import create_out_dir
from compute_error import compute_pose_estim_error, compute_stats
from visuals import export_box_plot, export_latex_table


if __name__ == "__main__":
    import argparse
    parser = argparse.ArgumentParser(description="Compute pose estimation accuracy")
    parser.add_argument(
        "--data_path",
        type=str,
        help="The path of log file with coordinates from AC",
        required=True,
    )
    parser.add_argument(
        "--out_path", type=str, help="The save path for resulted csv", required=False
    )
    args = parser.parse_args()
    dirs = create_out_dir(args.out_path)
    data = parse_log_data(data_path=args.data_path)
    results = compute_pose_estim_error(data=data)
    export_to_csv(data=results, out_path=dirs['csv'])
    stats_results = compute_stats(data=results)
    export_stats_to_csv(data=stats_results, out_path=dirs['csv'])
    export_box_plot(csv_path=dirs['csv'], out_path=dirs['boxplot'])
    export_latex_table(csv_path=dirs['csv'], out_path=dirs['latex'])




