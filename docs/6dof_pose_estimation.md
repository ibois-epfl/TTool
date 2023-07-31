1. [EfficientPose: An efficient, accurate and scalable end-to-end 6D multi object pose estimation approach ](https://paperswithcode.com/paper/efficientpose-an-efficient-accurate-and)
    - Extending the model from EfficientDet - an efficient net for object detection - to output the rotation and translation vector
    - The model is only compatible with one camera and limited number of models.
    - They use Linemod dataset and Occlusion dataset, which is a subset of linemod, for training the model (training a separated model for each dataset).

2. [6 DoF Pose Regression via Differentiable Rendering](https://link.springer.com/chapter/10.1007/978-3-031-06430-2_54)
    - Orignally, there are 2 approaches: direct regression and perspective-n-points (PnP)
        - The first one requires a lot of training data - which we may have, but model needs to remember the limited set of 3D models (becuase it needs to implicitly detect the type of the model and regress its 6DoF pose) (The model depends on spcific 3D models).
        - The second method requires 2D-3D correspondence points which imposes additional complication layer to our augmented carpentry.
    - Using differentiable rendering to do the coarse pose estimation and render-and-compare refinement.
    - Reconstructing a 3D model and then use that to generate more dataset for the model. **Reflection**: we don't really need to use the Blender if it is too complicated for deriving the pose as the camera is moving. We can also generate the dataset from all the 3D model that we have and the renderer from view.hh in the TTool. By doing this, we are simplifying the model by fixing the camera matrix of the renderer (which we will have to check and do the same thing if we use the data from the Blender).
    - With the model being rendered on the model, this method should work on any kind of object provided to it.
    - Coarse pose estimation is done by ResNet feature extraction shared between rotaion and translation regressor
    - Loss for translation and rotation are too simple.

- Blender 6DoF transformation - idea of transforming the object pose so it is still rendered the same image, but the camera's extrinsic matrix is fixed to another value.
    - invert the current camera pose T_cam_inv
    - apply the T_cam_inv to the object pose T_obj -> T_obj2
    - new camera's extrinsic matrix is T_cam2
    - apply T_cam2 to the T_obj2 -> T_obj3

- [PyTorch3D](https://pytorch3d.org/)
    - For loading model and rendering
    - Tools for deriving some metrics
    - Differentiable rendering :)

3. [End-to-End 6-DoF Object Pose Estimation Through Differentiable Rasterization ](https://link.springer.com/chapter/10.1007/978-3-030-11015-4_53)
    - The initial pose estimation is done with the CNN to predict both the class and the 6DoF at simultaneously.
    - The loss of the pose is the MSE of the element in the predicted pose and the true pose matrix.
    - The classification loss is the cross-entropy function (to which we don't have to pay attention)
    - The paper then refine the estimated pose with the differentiable renderer, but what is interesting and useful is that they use the silhouette of the 3D model. This means that the model does not need to have a accurate texture or the notion of the lighting which is much simpler than to have a realistic rendering method.

4. [LatentFusion: End-to-End Differentiable Reconstruction and Rendering for Unseen Object Pose Estimation](https://arxiv.org/abs/1912.00416)
    - [PyTorch project](https://github.com/NVlabs/latentfusion)
    
