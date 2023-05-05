#include <iostream>
#include <fstream>
#include <glog/logging.h>
#include <opencv2/core.hpp>
#include "global_param.hh"

namespace tk {

    GlobalParam *GlobalParam::instance = nullptr; /*!< an instance of a GlobalParam to act as a global variable */

    /**
     * Initialized all members to default values
     */
    GlobalParam::GlobalParam() = default;

    /**
     * Initialize the GlobalParam instance if not yet initialized
     * @return
     */
    GlobalParam *GlobalParam::Instance() {
        if (instance == nullptr) {
            instance = new GlobalParam();
        }
        return instance;
    }

    /**
     * Parse a value from config and put it to the field
     * @tparam T type of a member in a vector
     * @param fs parsed config YML file
     * @param idx name of the config key
     * @param field output value
     */
    template<typename T>
    void ReadValue(cv::FileStorage &fs, const std::string &idx, T &field) {
        cv::FileNode node = fs[idx];
        CHECK(!node.empty()) << "check " << '<' << idx << '>' << " in config file.";
        node >> field;
    }

    /**
     * Parse a vector from config and put it to the field
     * @tparam T type of a member in a vector
     * @param fs parsed config YML file
     * @param idx name of the config key
     * @param field output vector
     */
    template<typename T>
    void ReadArray(cv::FileStorage &fs, const std::string &idx, std::vector<T> &field) {
        cv::FileNode node = fs[idx];
        CHECK(!node.empty()) << "check " << '<' << idx << '>' << " in config file.";
        for (auto &&it: node) {
            field.push_back(T(it));
        }
    }

    /**
     * Parse YML to GlobalParam
     * Note: member of GlobalParam should correspond to the indices in config YML
     * @param config_file an absolute path to config file
     */
    void GlobalParam::ParseConfig(const std::string &config_file) {
        std::cout << "Parsing config file: " << config_file << std::endl;

        cv::FileStorage fs(config_file, cv::FileStorage::READ);
        CHECK(fs.isOpened()) << "failed to read config: " << config_file;

        cv::FileNode node;

        /////////////////////////////////////
        // camera
        /////////////////////////////////////
        ReadValue(fs, "camera_config_file", camera_config_file);
        ReadArray(fs, "K", K);
        ReadArray(fs, "dist_coeff", dist_coeff);
        ReadValue(fs, "preprocess_width", preprocess_width);
        ReadValue(fs, "preprocess_height", preprocess_height);

        /////////////////////////////////////
        // input
        /////////////////////////////////////
        ReadArray(fs, "model_file", model_file);
        ReadValue(fs, "unit_model", unit_model);

        ReadValue(fs, "frames", frames);
        ReadValue(fs, "camera_id", camera_id);

        ReadValue(fs, "fx", fx);
        ReadValue(fs, "fy", fy);
        ReadValue(fs, "cx", cx);
        ReadValue(fs, "cy", cy);

        ReadValue(fs, "zn", zn);
        ReadValue(fs, "zf", zf);

        /////////////////////////////////////
        // histogram
        /////////////////////////////////////
        ReadValue(fs, "hist_offset", hist_offset);
        ReadValue(fs, "hist_rad", hist_rad);
        ReadValue(fs, "search_rad", search_rad);
        ReadValue(fs, "alpha_fg", alpha_fg);
        ReadValue(fs, "alpha_bg", alpha_bg);

        /////////////////////////////////////
        // tracker
        /////////////////////////////////////
        ReadValue(fs, "line_len", line_len);
        ReadValue(fs, "sl_seg", sl_seg);

        /////////////////////////////////////
        // tracker
        /////////////////////////////////////
        ReadValue(fs, "tracker_mode", tracker_mode);
        ReadValue(fs, "timeout", timeout);
        ReadValue(fs, "target_frame", target_frame);
        ReadValue(fs, "show_result", show_result);

        /////////////////////////////////////
        // output
        /////////////////////////////////////
        ReadValue(fs, "input_pose_file", input_pose_file);
        ReadValue(fs, "tk_pose_file", tk_pose_file);
        ReadValue(fs, "fps_file", fps_file);
        ReadValue(fs, "result_video_file", result_video_file);
        ReadValue(fs, "result_img_dir", bench_case);

        /////////////////////////////////////
        // benchmark
        /////////////////////////////////////
        ReadValue(fs, "report_file", report_file);
        ReadValue(fs, "bench_mode", bench_mode);
        ReadValue(fs, "gt_pose_file", gt_pose_file);
    }

    /**
     * Dump a value of a given index to a YML line
     * @tparam T type of a field
     * @param os ostream to be dumped
     * @param idx index of the config to be dumped
     * @param field value of the config to be dumped
     */
    template<typename T>
    void DumpValue(std::ostream &os, const std::string &idx, const T &field) {
        os << idx << ": " << field << std::endl;
    }

    /**
     * Dump a value of a given index to a YML line
     * @param os ostream to be dumped
     * @param idx index of the config to be dumped
     * @param field string value of the config to be dumped (will be wrapped with double quote)
     */
    void DumpValue(std::ostream &os, const std::string &idx, const std::string &field) {
        os << idx << ": \"" << field << '\"' << std::endl;
    }

    /**
     * Dump an array of a given index to a YML line
     * @tparam T type of a field
     * @param os ostream to be dumped
     * @param idx index of the config to be dumped
     * @param field value of the config to be dumped
     */
    template<typename T>
    void DumpArray(std::ostream &os, const std::string &idx, const std::vector<T> &field) {
        os << idx << ": [ ";
        int array_size = field.size();
        for (int i = 0; i < array_size; ++i) {
            os << field[i];
            if (i != array_size - 1)
                os << ", ";
        }
        os << " ]" << std::endl;
    }

    /**
     * Overridden operator<< for GlobalParam
     * @param os ostream to be dumped
     * @param gp GlobalParam to be dumped (*this)
     * @return
     */
    std::ostream &operator<<(std::ostream &os, const GlobalParam &gp) {
        os << "%YAML:1.0" << std::endl;
        /////////////////////////////////////
        // input
        /////////////////////////////////////

        DumpValue(os, "unit_model", gp.unit_model);


        DumpValue(os, "frames", gp.frames);
        DumpValue(os, "camera_id", gp.camera_id);

        DumpValue(os, "fx", gp.fx);
        DumpValue(os, "fy", gp.fy);
        DumpValue(os, "cx", gp.cx);
        DumpValue(os, "cy", gp.cy);

        /////////////////////////////////////
        // tracker
        /////////////////////////////////////

        DumpValue(os, "tracker_mode", gp.tracker_mode);
        DumpValue(os, "line_len", gp.line_len);
        DumpValue(os, "alpha_fg", gp.alpha_fg);
        DumpValue(os, "alpha_bg", gp.alpha_bg);

        // field tracker
        DumpValue(os, "grid_step", gp.grid_step);
        DumpValue(os, "is_normalize_fields", gp.is_normalize_fields);
        DumpArray(os, "pyramid_variance", gp.pyramid_variance);

        /////////////////////////////////////
        // solver
        /////////////////////////////////////

        DumpValue(os, "solver_mode", gp.solver_mode);
        DumpValue(os, "delta_threshold", gp.delta_threshold);
        DumpValue(os, "error_threshold", gp.error_threshold);

        DumpArray(os, "scales", gp.scales);
        DumpArray(os, "iters", gp.iters);

        DumpValue(os, "lambda", gp.lambda);
        DumpValue(os, "u_rate", gp.u_rate);
        DumpValue(os, "d_rate", gp.d_rate);

        /////////////////////////////////////
        // output
        /////////////////////////////////////
        DumpValue(os, "out_path", gp.out_path);
        DumpValue(os, "video_file_on", gp.video_file_on);
        DumpValue(os, "pose_file_on", gp.pose_file_on);

        /////////////////////////////////////
        // benchmark
        /////////////////////////////////////
        DumpValue(os, "gt_pose_file", gp.gt_pose_file);
        DumpValue(os, "marker_len", gp.marker_len);
        DumpValue(os, "offset_x", gp.offset_x);

        return os;
    }

    /**
     * Dump the config to ostream in a YML format
     * @param os ostream to dump the config to
     */
    void GlobalParam::DumpConfig(std::ostream &os) const {
        os << *this;
    }

}