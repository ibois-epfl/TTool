# 2023.03.07 - TSegment integration
With the camera loop from TTool, `segmentation` from `TSegmentation` can be inserted using `consume_image(camera_ptr->image())`

Getting mask can be done using `get_mask()`.

Result
![](2023.03.07/TSegment-integration.gif)

## Next Step
- [x] TSegment according to [contribution style](https://github.com/ibois-epfl/augmented-carpentry/blob/main/CONTRIBUTING.md)
- [x] TSegment only exposed functions should be in public otherwise, private
- [x] TSegment [Docstring](https://github.com/ibois-epfl/TSlam/blob/main/src/reconstruction/ts_geometric_solver.hh)
- [X] TSegment passing ref (&) instead of a copy of image
- [x] TSegment fix on real camera
- [x] TSegment separate `compute mask` (public), `get mask` (public) function and getter for `cv::Mat mask` (private)
- [ ] Design the TTool dataflow