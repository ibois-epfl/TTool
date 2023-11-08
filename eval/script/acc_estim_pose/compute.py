from io_stream import parse_log_data, dump_results_to_csv, dump_stats_to_csv
from util import create_out_directory
from err_compute import compute_pose_estimation_accuracy, compute_stats


if __name__ == "__main__":
    # add the module error computation, and in compute we do everything
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
    save_path = create_out_directory(args.out_path)

    data = parse_log_data(args.data_path)
    results = compute_pose_estimation_accuracy(data)

    dump_results_to_csv(results, save_path)

    stats_results = compute_stats(results)

    dump_stats_to_csv(stats_results, save_path)



