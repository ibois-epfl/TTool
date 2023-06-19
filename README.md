<p align="center">
    <img src="./img/logo_linux_green.png" width="200">
</p>
<p align="center">
    <img src="https://github.com/ibois-epfl/TTool/actions/workflows/docker-cmake-build.yml/badge.svg">
    <img src="https://img.shields.io/badge/os-Ubuntu22.04-blueviolet">
    <img src="https://img.shields.io/badge/license-GPL--v3-brightgreen">
</p>

```mermaid
gantt
    dateFormat  YYYY-MM-DD
    title       NC package planning
    axisFormat %Y-%m

    Start                                                     :milestone, done, strt, 2023-06-01, 0d

    section Vocation
    french course half-day                                    :crit, 2023-06-12, 2023-06-23
    day off                                                   :crit, milestone, 2023-06-16

    section TTool standalone dev
    (1)dataset                                                   :crit, dtset, 2023-06-05, 12d
    (2)utilities (video export + visuals)                        :active, util, after dtset, 2d
    (3)API Ttool library refactoring                             :apirefct, after util, 10d

    section Ttool-AC integration
    (4)Ttool layer                                               :ttollay, after apirefct, 20d
    (5)Fabrication testing + unit testing                        :ftref, after ttollay, 5d

    section ML 2D dev
    (*)classifier prototype with real data                       :classrdata, 2023-06-12, 2023-06-24
    (6)2d detection - state of the art                           :2ddetsota, after apirefct, 4d
    (7)2d detection - refactoring synth generator                :2ddetref, after 2ddetsota, 10d
    (8)2d detection - training                                   :2ddettra, after 2ddetref, 5d
    (9)2d detection - refinement/evaluation                      :2ddetrefin, after 2ddettra, 5d
    (10)2d detection - integration API                           :2ddetinteg, after 2ddetrefin, 10d
    (11) 3d pose detector                                        :crit, 3dposde, after 2ddetinteg, 12d

    section optimisation
    (**)tSLET optimisation                                        :crit, optim, after 2ddetinteg, 12d

    code refinement                                           :ref, after 3dposde, 4d
    documentation/cleaning                                    :docclean, 2023-08-10, 2023-08-14

    End                                                       :milestone, ender, 2023-08-14, 0d
```

## NC description working packages
- [ ] **(1) dataset**: the dataset needs to be finished and uploaded on zenodo. Here's the upcomong tasks:
  - [ ] chainsaw blade to scan
  - [ ] long drillb its (x2) to scan
  - [ ] refactor and reorder the dataset
  - [ ] add to the dataset: circular sawblade, saber sawblade, (optional) drillbit, check if it exists
  - [ ] refactor dataset nameing: <englishname>_<widthmm>_<lengthmm> (e.g. spadedrill_25_115)
  - [ ] integrate downloading procedures in AC and TTool

- [ ] **(2) utilities**: all utilities for the documentation of the developement needs to be implemented:
  - [x] video recorder (with and without gui images)
  - [ ] camera pose output
  - [ ] log output with fabrication info

- [ ] **(3) API Ttool library refactoring**: the TTool needs to be refactored to an API. The executable should only use calls from the API. The API will be integrated to AC in the package later.

- [ ] **(4) Ttool layer**: in AC the tool header recognition needs to occupy a layer folling the AC structure and calls from the TTool API take place unikely there. It is possible that some level of multithreading (?) or corutine needs to be implemented to avoid bottlenecks in the main AC rander thread.

- [ ] **(5) Fabrication testing + unit testing**: the ttol layer in tegration in AC needs to be tested in fabrication scenario and features and modifications will be added following observations and quick user tests.

- [ ] **(6) 2d detection - state of the art**: some days will be reserved to an extensive review of the existing code that can be used in our application. Once the model identified it will be vet and discussed before starting the development.

- [ ] **(7-8-9-10) 2d detection - generator/training/refinement**: classical ML development for the custom training based on pre-existing synthetic data generator. The generator will need to be refacotered to add:
  - [ ] general simplifications
  - [ ] connection to the open-source dataset
  - [ ] procedural background images
  - [ ] procedural lighting
  - [ ] format output based on the chosen training format

- [ ] **(11) (optional) 3d pose detector**: if we are on time, 2 weeks will be reserved to the integration of a initial 6DOF pose detector to avoid the manual placing of the object.

- [ ] **optimisation**:
  - [ ] [Track's RunIteration](./src/tracker_sle.cc#L185) has fixed number of run
  - [ ] Search line depends on the size of the Object3D. This leads to slower [EstimatePose](./src/tracker_sle.cc#L306) on some models

---

# TTool
This is the repository hosting the API for TTool. It is a program able to detect the 6dof of a fix toolhead from the feed of a fix camera view.

## Config & Build
```bash
cmake -S . -B build
cmake --build build
```
# Install/uninstall
```bash
sudo cmake --install build
```
```bash
sudo cmake --build build --target uninstall
```
## Run executable

```bash
cmake -S . -B build && cmake --build build && ./build/ttool -c 0 -l "./assets/calibration_orange_A_1280_720_r.yml" -t -s
```
where,

- `[-h,--help]`: print this message
- `[-c,--camera]`: camera index
- `[-l,--calib]`: calibration file for the camera
- `[-s,--save]`: record video of session, give custom path for output (default: ./default)
- `[-t,--trackPose]`: it saves all poses and objects in a log file


## CI/CD
If you commit and push some code that does not influence the compilation (e.g. readme, docs, etc), add one of these texts to your commit message, it will stop the run of the github action.
```
[skip ci]
[ci skip]
[no ci]
[skip actions]
[actions skip]
```
