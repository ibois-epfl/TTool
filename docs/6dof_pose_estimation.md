1. [EfficientPose: An efficient, accurate and scalable end-to-end 6D multi object pose estimation approach](https://paperswithcode.com/paper/efficientpose-an-efficient-accurate-and)
    - Extending the model from EfficientDet - an efficient net for object detection - to output the rotation and translation vector
    - The model is only compatible with one camera and limited number of models.
    - They use Linemod dataset and Occlusion dataset, which is a subset of linemod, for training the model (training a separated model for each dataset).
    - Better than DROD+ even without the refiner module
    - The refiner module is built-in inside the model
    - Has a summary of add ADD acuuracy of several SOTA (YOLO6D, Pix2Pose, PVNet, DPOD, DPOD+, CDPN, HybridPose)

2. [6 DoF Pose Regression via Differentiable Rendering](https://link.springer.com/chapter/10.1007/978-3-031-06430-2_54)
    - Orignally, there are 2 approaches: direct regression and perspective-n-points (PnP)
        - The first one requires a lot of training data - which we may have, but model needs to remember the limited set of 3D models (becuase it needs to implicitly detect the type of the model and regress its 6DoF pose) (The model depends on spcific 3D models).
        - The second method requires 2D-3D correspondence points which imposes additional complication layer to our augmented carpentry.
    - Using direct regression to do the coarse pose estimation and differentiable rendering to do render-and-compare refinement.
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
    - The initial pose estimation is done with the PoseCNN to predict both the class and the 6DoF at simultaneously.
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
    - In 2, just the loss based on the rendering from the pose
    
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

6. [DeepIM: Deep Iterative Matching for 6D Pose Estimation](https://arxiv.org/pdf/1804.00175v4.pdf)
    - Using iterative matching to refine the inital pose
    - Initial pose is estimated by the PoseCNN
    - Using FlowNet as a backbone of the DeepIM to predict the pose
    - During the training, FlowNet was branched out to predict the next optical flow between the observed image and the rendered image, while the other branch predicts the foreground mask of the observed image
    - Modified version of Geometric reprojection loss instead of Angular distance for rotation loss and L2 loss for translation loss as they are more difficulty to balance

7. [SO-Pose: Exploiting Self-Occlusion for Direct 6D Pose Estimation](https://paperswithcode.com/paper/so-pose-exploiting-self-occlusion-for-direct)
    - End-to-end (directly predict the pose) is inferior than the two-stage (2D-3D correspondence + RANSAC & PnP solver)
    - Leveraging the self occlusion information about the object (occluded part of the object that is occluded by itself).
    - Using off-the-shelf object detector and crop the image before feeding it to the network.
    - Two decoders process the image features. One for predicting the correspondences and the object mask. The other for the self-occlusion map.
    - Finally, feeding both self-occlusion, correspondences and the mask to the pose estimator.
    - The loss function includes pose, cross-layer consistency, and self-occlusion term. Hence, needing the groundtruth self-occlusion coordinates.
8. [GDR-Net: Geometry-Guided Direct Regression Network for Monocular 6D Object Pose Estimation](https://arxiv.org/abs/2102.12145)
    - Much similar to the SO-Pose (whereas SO-Pose claims to be better). Using off-the-shelf object detector and crop it to the network.
    - It it end-to-end method, but explicity predict the 2D-3D correspondences before passing to the regressor network. Hence, leveraging the best of both world.
    - 6D Rotation to represent the rotation as many rotation representation are ambiguous (two different matrices representing the same rotation).
    - Normally, translation represented with (o_x, o_y) of the projected 3D centroid and the object's distance t_z towards the camera. (translation relative to camera, see how to calculate back the T with camera intrinsics K), because the image is zoomed in, the translate is represent with scale-invariant representation for (Translation Esimation, SITE).
    - Loss function of the pose estimation is the distangled calculation of the Rotation loss, X-Y scale-invariant translation loss and the, Z scale-invariant translation loss. + Symmetry-aware formulation for the rotation under symmetry.

9. [RNNPose: Recurrent 6-DoF Object Pose Refinement with Robust Correspondence Field Estimation and Pose Optimization](https://paperswithcode.com/paper/rnnpose-recurrent-6-dof-object-pose)
    - This is a pose refinement method.
    - The essential part is that it estimates the inital pose with PVNet or PoseCNN

10. [Real-Time Seamless Single Shot 6D Object Pose Prediction](https://paperswithcode.com/paper/real-time-seamless-single-shot-6d-object-pose)
    - 2D-3D correspondences + PnP solver method
    - Claim to be precise enough that no further refinement is needed.
    - (Just for our intro) Traditional 2D-3D method requires textured object, and learning-based 2D-3D method addresses this issue.
    - Predicting 9 points (8 corners of 3D bounding box and 1 centroid). Hence, does not need the 3D model
    - Architecture like YOLOv2 (thus, also fast inference time)

11. [PVNet: Pixel-wise Voting Network for 6DoF Pose Estimation](https://arxiv.org/abs/1812.11788)
    - 2D-3D correspondences + PnP solver method (two-stage pipeline)
    - The PVNet outputs the keypoints with the confidence level, allowing the uncertainty-driven PnP algorithm to leverage the value.
    - PVNet outputs
        - sematic label associating the pixel with a specific object
        - unit vector representing the direction from the pixel point p to a 2D keypoint x_k of the object.
        -With RANSAC-based, these two output yield keypoint hypotheses.
    - Loss function for learning unit vectors (the network only learns from this loss function)

12. [PoseCNN: A Convolutional Neural Network for 6D Object Pose Estimation in Cluttered Scenes](https://arxiv.org/pdf/1711.00199.pdf)
    - End-to-end pose estimation and very robust to occlusion
    - ShapeMatch-loss to address symmetric objects + Pose Loss for other rotational loss
    - The network does 3 things: semantic segmentation, center box prediction (for translation given camera intrinsic), and rotaion prediction
    - PoseCNN outperforms the 3D coordinate regression network
    - Using ADD and ADD-S as the metrics for evaluating
    - ADD metric is the average of the projection euclideean distance of 3D points in the model between the ground truth and the estimation
    - ADD-S metric is the average of the **minimum** projection euclideean distance of 3D points in the model between the ground truth and the estimation. The use of minimum is to address the symmetric model.

13. [SegICP: Integrated Deep Semantic Segmentation and Pose Estimation](https://www.cs.princeton.edu/~jw60/wong2017segicp.pdf)
    - It is using RGB-D which is not of our interest.
    - It introduced the model-to-scene alignment metric between the point clouds of the model and the estimated point clouds. This is interesting because it has been used in the refinement of the PoseCNN, however still with depth. Nevertheless, the should inspire us to use the alignment idea have another pseudo-goodness metric of the SLET.
        - a(M, S) = |c| / |M
        - M is the set of points in the in the crop point cloud
        - S is the set of points in the segmented object scene cloud
        - c is the set of points in M with unqiue corresponding points in S at most τ metwers away.
        - computation is done with the help of kd-tree 

14. [DPOD: 6D Pose Object Detector and Refiner](https://arxiv.org/pdf/1902.11020.pdf)
    - 2D-3D correspondence map with refiner
    - Surpassing the PVNet method but refiner is needed
    - Has a summary of add ADD acuuracy of several SOTA (YOLO6D, PoseCNN, PVNet, DeepIM)

15. [ROPE: Occlusion-Robust Object Pose Estimation with Holistic Representation](https://arxiv.org/pdf/2110.11636v1.pdf)
    - 2D-3D correspondence map
    - Enforcing robustness against occlusion without the need ot pose refinement via data augmentation
    - Need 3D point cloud of the target object
    - Surpassing SoTA with refinement

16. [SSD-6D: Making RGB-based 3D detection and 6D pose estimation great again](https://openaccess.thecvf.com/content_ICCV_2017/papers/Kehl_SSD-6D_Making_RGB-Based_ICCV_2017_paper.pdf)
    - https://github.com/wadimkehl/ssd-6d - code for training and refinement cannot be made available
    - End-to-end approach
    - Can just do the inference, no training or fine-tuning
