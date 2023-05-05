# Roadmap week-by-week

- [ ] - Reset / lower threshold 
- [x] *(URGENT)* Load the distortionCoeff, and the K parameters from the path, tracker should use the K, and distCoeff from the same vars as cameraPtr
- [ ] *(URGENT)* load calibration distortion matrix from external zml file instead of hardcoded *be sure that all across the source code the same calibration file is consumed* see https://github.com/ibois-epfl/TSlam/blob/88e61aee7063de8da604e5488d5b65a138f2878a/utils/tslam_reconstruct.cc
- [ ] branch with segmentation, main without segmentation
- [ ] change to local axis for translation and orientation manipulation for the pose input (as in blender)
- [ ] Reset it to text initial pose User *we have reset to text gt pose and reset to checkpoint pose* 
- [ ] Stop and freeze ('q' during the tracking) 50%
- [ ] Overwrite the initial pose wiht one keymap User
- [ ] Reset the color histogram when rerun
- [ ] Input write locally the pose and the mask for each tool head


- [ ] Visualization keymap (help keystroke overlay)
- [ ] Integrate the download in the github repository (CMake config download links)
- [ ] Complete all the datasets, and place it on another repository

- [ ] get args from the terminal, with default param from the yaml config file from SLET "/home/tpp/TTool/live_camera/config.yml"


- [ ] Debugging of all the viewers *(Fragmentation nd contour views and stuff into the video)
- [ ] (Optional) visualize bbox
- [x] Try with the real calibration

On hold
- [ ] Clear cache segmentation
- [ ] Switching between mask and camera feedqp
- [ ] Try manual segmentation mask for the circular saw
- [x] show segmentation
Note; if we don't allow the drill to reset, it worked better