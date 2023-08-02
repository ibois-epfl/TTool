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
    - [Their PyTorch project](https://github.com/NVlabs/latentfusion), because the link in the paper does not work anymore.
    - Taking a set of reference RGB images of a target object with known poses, and internally builds a 3D representation of the object.
    - It is better for generalization to unseen object poses during testing because it has the 3D reconstruction under the hood.
    - The loss is the conbination of depth, mask and latent representation loss.
    - The pipeline is divided into 2 main parts: 1) neural reconstruction and render, and 2) Pose estimation
    - In 1, sample 16 reference poses and 16 target poses. Using image-based rendering with depth map and binary mask.
    - In 2, 
    
5. [Deep Object Pose Estimation for Semantic Robotic Grasping of Household Objects](https://arxiv.org/abs/1809.10790)
    - Using only the synthetic data to train the model.
    - Deep learning model to estimate the keypoints, and then use the PnP algorithm to estimate the pose.
    - Keypoints are estimated using FCN to output "belief maps" and "vector fields".
    - In total of 9 belief maps, 8 are for 3D bounding box vectices, 1 for the centroid.
    - 8 vector fields indicate the direction from each of the 8 vertices to the centroid. Thus, allowing the model to detect multiple objects.
    - Detection and pose estimation were done with the thresholding and greedy algorithm to select the keypoints and do the PnP.
    - "Photorealistic data" - using the 3D background
    - [LabelFusion](https://arxiv.org/abs/1707.04796) - tool for 3D bounding box labeling for the RGBD data
    - All data were generated by a custom plugin we developed for Unreal Engine 4 (UE4) called NDDS

6. [DeepIM: Deep Iterative Matching for 6D Pose Estimation](https://arxiv.org/abs/1804.00175)
    - Render the 3D model to match the image.