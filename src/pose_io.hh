#include <fstream>
#include <iomanip>
#include <string>
#include <vector>
#include <math.h>

#include "util.hh"

void print_vector(std::vector<std::string> v) {
    for (auto i=0; i<v.size(); ++i) {
        std::cout << v[i] << "; ";
    }
    std::cout << "\n";
}

class PoseReader {
public:
	virtual void Read(const std::string& pose_file, std::vector<std::vector<cv::Matx44f> >& poses) = 0;
	virtual void Read(const std::string& pose_file, std::vector<int>& fids, std::vector<std::vector<cv::Matx44f> >& poses) {
		CHECK(false) << "ERROR: NOT implement";
	}
};

class PoseReaderRBOT: public PoseReader {
public:
	static float get_errorR(cv::Matx33f R_gt, cv::Matx33f R_pred) {
		cv::Matx33f tmp = R_pred.t() * R_gt;
		float trace = tmp(0, 0) + tmp(1, 1) + tmp(2, 2);
		return acos((trace - 1) / 2.0f) * 180.0f / 3.14159265f;
	}

	static float get_errort(cv::Vec3f t_gt, cv::Vec3f t_pred) {
		float et0 = t_gt[0] - t_pred[0];
		float et1 = t_gt[1] - t_pred[1];
		float et2 = t_gt[2] - t_pred[2];
		return sqrt(et0 * et0 + et1 * et1 + et2 * et2);
	}

	static void ParseMat(const std::string& pose_str, int& fid, cv::Matx44f& pose) {
        std::stringstream ss_pose_str(pose_str);
        float tmp;
        std::vector<float> quat_vals;
        while(ss_pose_str >> tmp){
            quat_vals.push_back(tmp);
        }

		float m00, m01, m02, m03;
		float m10, m11, m12, m13;
		float m20, m21, m22, m23;

		if (quat_vals.size() == 13) {
			fid = int(quat_vals[0]);

			m00 = quat_vals[1];
			m01 = quat_vals[2];
			m02 = quat_vals[3];
			m10 = quat_vals[4];
			m11 = quat_vals[5];
			m12 = quat_vals[6];
			m20 = quat_vals[7];
			m21 = quat_vals[8];
			m22 = quat_vals[9];
			m03 = quat_vals[10];
			m13 = quat_vals[11];
			m23 = quat_vals[12];
		}

		if (quat_vals.size() == 12) {
			m00 = quat_vals[0];
			m01 = quat_vals[1];
			m02 = quat_vals[2];
			m10 = quat_vals[3];
			m11 = quat_vals[4];
			m12 = quat_vals[5];
			m20 = quat_vals[6];
			m21 = quat_vals[7];
			m22 = quat_vals[8];
			m03 = quat_vals[9];
			m13 = quat_vals[10];
			m23 = quat_vals[11];
		}

		cv::Matx44f mat(
			m00, m01, m02, m03,
			m10, m11, m12, m13,
			m20, m21, m22, m23,
			0, 0, 0, 1);
		pose = mat;
	}

	virtual void Read(const std::string& pose_file, std::vector<std::vector<cv::Matx44f> >& poses) override {
		std::ifstream fs;
		fs.open(pose_file);
		std::cout << "DEBUG_READ_1 " << fs.is_open() << std::endl;  // <--- DEBUG
		if (!fs.is_open()) {
			std::cerr << "failed to open pose file, press any key to exit" << std::endl;
			getchar();
			exit(-1);
		}

		std::string line;

		std::getline(fs, line);
		std::getline(fs, line);
		int obj_size = atoi(line.c_str());

		std::cout << "DEBUG_READ_22::LINE= "<< line << std::endl;  // <--- DEBUG
		std::cout << "DEBUG_READ_23::OBJECT_SIZE= "<< obj_size << std::endl;  // <--- DEBUG

		poses.resize(obj_size);

		while (true) {
			if (fs.eof()) break;

			for (int i = 0; i < obj_size; ++i) {
				std::getline(fs, line);
				if (line.size() < 4) {
					//std::cout << "pose file to short" << std::endl;
					break;
				}

				cv::Matx44f pose;
				int fid;
				ParseMat(line, fid, pose);
				
				poses[i].push_back(pose);
			}
			// std::cout << "DEBUG_READ_3" << std::endl;  // <--- DEBUG

		}
	}

	virtual void Read(const std::string& pose_file, std::vector<int>& fids, std::vector<std::vector<cv::Matx44f> >& poses) override {
		std::ifstream fs;
		fs.open(pose_file);
		if (!fs.is_open()) {
			std::cerr << "failed to open pose file, press any key to exit" << std::endl;
			getchar();
			exit(-1);
		}

		std::string line;

		std::getline(fs, line);
		std::getline(fs, line);
		int obj_size = atoi(line.c_str());

		poses.resize(obj_size);
		fids.clear();

		while (true) {
			if (fs.eof()) break;
			
			int fid;
			for (int i = 0; i < obj_size; ++i) {
				std::getline(fs, line);
				if (line.size() < 4) {
					//std::cout << "pose file to short" << std::endl;
					break;
				}

				cv::Matx44f pose;
				ParseMat(line, fid, pose);

				poses[i].push_back(pose);
			}
			fids.push_back(fid);
		}
	}

	void Read(const std::string& pose_file, std::string& config_file, std::vector<int>& fids, std::vector<cv::Matx44f>& poses) {
		std::ifstream fs;
		fs.open(pose_file);
		if (!fs.is_open()) {
			std::cerr << "failed to open pose file, press any key to exit" << std::endl;
			getchar();
			exit(-1);
		}

		std::string line;
		std::getline(fs, line);

		config_file = line;

		while (true) {
			if (fs.eof()) break;
			std::getline(fs, line);
			if (line.size() < 4) {
				std::cout << "pose file to short" << std::endl;
				break;
			}

			cv::Matx44f pose;
			int fid;
			ParseMat(line, fid, pose);

			fids.push_back(fid);
			poses.push_back(pose);
		}
	}
};
