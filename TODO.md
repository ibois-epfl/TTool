# Roadmap week-by-week

- [x] - Reset / lower threshold 0.025 -> 0.0125
- [x] *(URGENT)* Load the distortionCoeff, and the K parameters from the path, tracker should use the K, and distCoeff from the same vars as cameraPtr
- [x] *(URGENT)* load calibration distortion matrix from external zml file instead of hardcoded *be sure that all across the source code the same calibration file is consumed* see https://github.com/ibois-epfl/TSlam/blob/88e61aee7063de8da604e5488d5b65a138f2878a/utils/tslam_reconstruct.cc
- [x] branch with segmentation, main without segmentation
- [x] change to local axis for translation and orientation manipulation for the pose input (as in blender) (seems like local translation does not make any change, so only did rotation)
- [x] Reset it to text initial pose User *we have reset to text gt pose and reset to checkpoint pose* 
- [x] Stop and freeze ('q' during the tracking) ('y' during tracking will write the pose to gt_pose and stop the tracking) 50%
- [x] Overwrite the initial pose with one keymap User ('y')
- [x] Reset the color histogram when rerun -> histogram get make of the projected frame and use that for the new histogram. each time the new tracking occur, the histogram is updated to the user input pose before the tracking start. So technicall it has already been done.
- [x] Input write locally the pose and the mask for each tool head

- [ ] Visualization keymap (help keystroke overlay)
- [ ] Integrate the download in the github repository (CMake config download links)
- [ ] Complete all the datasets, and place it on another repository

- [ ] Debugging of all the viewers *(Fragmentation nd contour views and stuff into the video)
- [ ] (Optional) visualize bbox
- [x] Try with the real calibration

On hold
- [ ] Clear cache segmentation
- [ ] Switching between mask and camera feedqp
- [ ] Try manual segmentation mask for the circular saw
- [x] show segmentation
Note; if we don't allow the drill to reset, it worked better