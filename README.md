<p align="center">
    <img src="./img/logo_linux_green.png" width="200">
</p>
<p align="center">
    <img src="https://github.com/ibois-epfl/TTool/actions/workflows/docker-cmake-build.yml/badge.svg">
    <img src="https://img.shields.io/badge/os-Ubuntu22.04-blueviolet">
    <img src="https://img.shields.io/badge/license-GPL--v3-brightgreen">
</p>

# TTool
This is the repository hosting the API for TTool. It is a program able to detect the 6dof of a fix toolhead from the feed of a fix camera view.

```mermaid
flowchart TB
    A(TTool API)
    B(TSegment API) --> A
    C(TSlet API) --> A
    D(ML API*) --> A

    A --> E(Augmented Carpentry)
```

## TTool Dataflow

```mermaid
graph TD;
cam[TTool::Camera]
uii["TTool::PoseInput / ML::PoseEstimator"]
seg[TSegment::Segmentation]
%% mle[ML::PoseEstimator]
tkr[TSLET::ObjectTracker]

cam--image-->seg
cam--image-->uii

seg--segmentationMask-->uii
seg--segmentationMask-->tkr


uii--initialPose-->tkr
uii--toolheadID-->tkr

%% uii<-->mle 

tkr--refinedPose-->TTool::Visualizer
tkr--3DObject-->TTool::Visualizer
```

## CI/CD
If you commit and push some code that does not influence the compilation (e.g. readme, docs, etc), add one of these texts to your commit message, it will stop the run of the github action.
```
[skip ci]
[ci skip]
[no ci]
[skip actions]
[actions skip]
```