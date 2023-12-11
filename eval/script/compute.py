import util
import visuals
import io_stream
import compute_error


def main(data_path: str, out_path: str, dir_name: str) -> None:
    print("\033[1m\033[44m[INFO]: __Pose Estimation Accuracy Error Computation Started__\033[0m")
    dirs = util.create_out_dir(out_path, dir_name)
    print(f"\033[92m[INFO]: Output directories created! {dirs}\033[0m")
    data = io_stream.parse_log_data(data_path=data_path)
    results = compute_error.compute_pose_error(data=data)
    print(f"\033[92m[INFO]: Pose estimation accuracy error computed!\033[0m")
    print(f"\033[92m[INFO]: Results will be exported to {dirs['csv']}\033[0m")
    io_stream.export_to_csv(data=results, out_path=dirs['csv'])
    stats_results = compute_error.compute_stats(data=results)
    io_stream.export_stats_to_csv(data=stats_results, out_path=dirs['csv'])
    print(f"\033[92m[INFO]: Boxplots will be exported to {dirs['boxplot']}\033[0m")
    visuals.export_box_plot(csv_path=dirs['csv'], out_path=dirs['boxplot'])
    print(f"\033[92m[INFO]: Latex tables will be exported to {dirs['latex']}\033[0m")
    visuals.export_latex_table(csv_path=dirs['csv'], out_path=dirs['latex'])
    print("\033[1m\033[44m[INFO]: __Pose Estimation Accuracy Error Computation Completed__\033[0m")


if __name__ == "__main__":
    import argparse
    parser = argparse.ArgumentParser(description="Compute pose estimation accuracy error")
    parser.add_argument("--data_path",type=str, help="The path of log file with coordinates from AC",
                        required=True,
    )

    parser.add_argument("--out_path", type=str, help="The save path for the output files (csv, boxplot, latex)",
                        required=True
    )

    parser.add_argument("--dir_name", type=str, help="The name of the directory to save the output files",
                        required=True
    )
    args = parser.parse_args()
    main(data_path=args.data_path, out_path=args.out_path, dir_name=args.dir_name)





