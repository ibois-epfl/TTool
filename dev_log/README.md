# 2023.03.07 - TSegment integration
With the camera loop from TTool, `segmentation` from `TSegmentation` can be inserted using `consume_image(camera_ptr->image())`

Getting mask can be done using `get_mask()`.

Result
![](2023.03.07/TSegment-integration.gif)