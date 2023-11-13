<p align="center">
    <img src="./img/logo_linux_green.png" width="200">
</p>
<p align="center">
    <img src="https://github.com/ibois-epfl/TTool/actions/workflows/docker-cmake-build.yml/badge.svg">
    <img src="https://img.shields.io/badge/os-Ubuntu22.04-blueviolet">
    <img src="https://img.shields.io/badge/license-GPL--v3-brightgreen">
</p>

# TTool
This is the repository hosting the API for TTool. TTool detects the 6dof of a fix toolhead from the feed of a fix camera view.
The contributions of this project are
- Making this program be both API and a standalone program.
- Model manager that manages different 3D models for 6DoF tracking. This allows user to change the model to be tracked at ease.
- Input pose to manipulate the 3D models for the initial pose. The initial pose is required by the SLET to begin the tracking. With input pose, the user can give a rough pose of the 3D, and have the SLET refine the pose afterward.
- Config manager to read the config YAML and managing I/O for saving the poses of the models for later.
- GLEW's OpenGL rendering. SLET is using QT5's OpenGL rendering. However, our [augmented carpentry](https://github.com/ibois-epfl/augmented-carpentry) is using GLFW. Hence, we changed the rendering back-end to suit our project.
- RESET, FREEZE, TRACK implementation. We found the correlation between the average score of the search points and the quality of the tracking. We added a thresholding to the SLET to exploit this score. As a consequece, the tracking works better on our setting of a fixed camera mounting on the toolhead.
- ML Tool head classification to detect the type of the tool from the camera.
- 
# Wiki
The wiki of this project is hosted on [github](https://github.com/ibois-epfl/TTool/wiki)
