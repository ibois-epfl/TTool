1. [Review of Image Classificaion Algorithm Based on Convulutional Neural Network](https://www.mdpi.com/2072-4292/13/22/4712)
    - Standard SOTAs are ResNet EfficientNet and Inception.
    - Some CNNs also integrate attention mechanism such as
        - Residual Attention Network - using spatial feature attention which is good for cluttered images.
        - SENet - addition channel-attention to any other model
        - BAM and CBAM utilize both spatial and feature attention
        - GNET
        - SKNet (improved by ECA-Net)
        - GSoP-Net
        - Corrdinate Attention (designed for mobile network)
    - Some CNNs try to make the model smaller for smaller computing devices
        - Network pruning
        - Knowledge distillation (KD)
    - Recently, we are moving towards Vision Transformer, but not covered in this paper
    - **Reflection**: we should try with the simple SOTAs because of their abundance in implementation in all the deep learning frameworks. If the performance is unacceptable we could experiment with attention mechanism and ViT
2. [Deformable DETR: Deformable Transformers for End-to-End Object Detection](https://arxiv.org/abs/2010.04159)
    - Comprehensive Transformer in Image classification
    ViT - memory complex because of self + cross attention
        - Some fix with pre-defined sparsed attention patterns on keys
        - data-dependent sparse attention such as locality sensitive hashing (LSH) based attention - hash both query and keey elements to different bins
        - Low-rank property in self-attention - reduces the number of keys through some ways
    - Deformable attention module - second category 
    - Multi-scale Feature Representation  may also help the model detect object at different scale - but in our case this may be sufficiently solved by data augmentation because our camera is fixed unlike object detection
    - **Reflection**: if we have to use ViT, we should also first, try with ViT and determine if the memory complexity actually affects our computing device or not. If so, we could proceed with the paper to make the architecture more memory-efficient.
3. [Big Transfer (BiT): General Visual Representation Learning](https://arxiv.org/pdf/1912.11370.pdf)
    - Transfer Learning from one model to the other with different dataset.
    - BiT model is Reset, replaced BN with GroupNorm.
    - It was trained on the "upstream" dataset before fintuned to the downstream task.
    - **Reflection**: The idea of transfer learning is interesting to us, because we could produce a lot of synthetic data from the 3D model that we have and also the pipeline to do so automatically.
4. [Efficient Video Classification Using Fewer Frames](https://arxiv.org/pdf/1902.10640.pdf)
    - Video Classification with Teacher and Student model RNN
    - Student model - so that the model is smaller 
    - RNN - for the consuming the video
    - **Reflection**: Live camera and the video classification seems also to be the good idea on our project. This is potentially another way that we can improve our model by leveraging multiple frames. However, again it should only be done if the basic SOTAs are not performing well.
5. [CNN-RNN: A Unified Framework for Multi-label Image Classification](https://arxiv.org/ftp/arxiv/papers/1604/1604.04573.pdf)
    - Use the CNN for extracting feature from an image.
    - Use the RNN for capturing the previously predicted labels. Hence, allowing the model to predict the other labels.
    - This unification allows the model to predict multiple labels from an image.
    - **Reflection**: Although interesting, we only need one label per image.
6. [Mechanical Tools Classification Dataset](https://www.kaggle.com/datasets/salmaneunus/mechanical-tools-dataset)
    - Public dataset for mechanical tool classification.
    - The labels include pliers, rope, toolbox, hammer and screwdriver.
    - The highest performing model in the kaggle is the Inception V3
    - **Reflection**: This dataset might be useful for our classification project. However, the labels are too small and not very extensible to our tool. The pretraining on this model might be helpful, but we could generate the synthetic data ourselves and make use of them instead.
7. [A Comprehensive Study on Robustness of Image Classification Models: Benchmarking and Rethinking](https://arxiv.org/pdf/2302.14301.pdf)
    - Image classification robustness, natural robustness, adversarial training.
    - Just need to make sure that we add the data augmentation to the image. 
    - We can also first check by simply flipping the image and see the model prediction result. 
    - **Reflection**: For us it would also be that during the classification may be there are some other toolheads lying around, but this should be evaluated on the real video instead.
8. [Explainable machine learning in image classification models: An uncertainty quantification perspective](https://www.sciencedirect.com/science/article/pii/S095070512200168X)
    - Image classification explainability. 
    - Entrophy based allows the model to say I don't know. 
    - Prediction Difference Analysis (PDA) to indicate the pixel contributing to the uncertainty. 
    - Handles those pixel with the proposed algorithm
    - **Reflection**: Explainability is useful when complimented with the quantitative results of the models. We should have some model explainability to check the model robustness visually.
9. [Learning Deep Features for Discriminative Localization](http://cnnlocalization.csail.mit.edu/Zhou_Learning_Deep_Features_CVPR_2016_paper.pdf)
    - Implicit localization on an image classification model. 
    - Simple and effective approah, but it is limited to some architectures. 
    - (For more architectures with CNN, Grad-CAM is used. However, it requires the gradient which is trickier).
    - **Reflection**: CAM seems the be more suitable to extract the explainability of the model from our basic SOTAs, namely EfficientNet.
10. [Recipro-CAM: Fast gradient-free visual explanations for convolutional neural networks ](https://paperswithcode.com/paper/recipro-cam-gradient-free-reciprocal-class)
    - Same intrinsic localization on the CNN for implicit explainability. 
    - Gradient free with the same mapping as Grad-CAM which makes the localization faster than the Grad-CAM. 
    - ADCC is the metric for evaluating CAM localization.
    - **Reflection**: This might prove useful if we need to try some other model.
11. [MobileViT: Light-weight, General-purpose, and Mobile-friendly Vision Transformer](https://machinelearning.apple.com/research/vision-transformer)
    - a light-weight and general-purpose vision transformer for mobile devices. 
    - Using linear project on the patches instead of CNN. 
    - Transformer as a convolution 
    - **Reflection**: This might prove useful if we need to try some other model (if we have to use ViT and it has memory issue on our computing devices).
12. [Model soups: averaging weights of multiple fine-tuned models improves accuracy without increasing inference time](https://paperswithcode.com/paper/model-soups-averaging-weights-of-multiple)
    - Averaging the weights of multiple fine-tuned models at a difference hyperparameters. 
    - Improve the accuracy without increasing inference time.
    - Averaging coefficient is based on the validation accuracy
    - **Reflection**: This method prove to be useful as the combine multiple models of the same architecture. Thus, the resulting model has the same size as one model and not imposing additional computational and memory cost at the inference time.
13. [Learning Transferable Visual Models From Natural Language Supervision](https://arxiv.org/pdf/2103.00020.pdf)
    - Pre-training image and text encoders, so that the cosine similarity of the encoded vectors of the two is high when the text and image belong to each other, and low otherwise. 
    - This is called contrastive representation learning in CLIP. 
    - CLIP shows potential in zero-shot transfer to other dataset. 
    - **Reflection**: This means that CLIP could learn some text representation on the visual information which may be useful for our data that will be increasing. But we need to see how CLIP does when it is finetuned to a domain-specific dataset (such as out case).
14. [Open-Vocabulary Semantic Segmentation with Mask-adapted CLIP](https://arxiv.org/pdf/2210.04150.pdf)
    - Mask proposal generator with MaskFormer. 
    - No weight change in CLIP. 
    - Only finetune the MaskFormer. (here it includes some dataset for zero shot segmentation). 
    - Use masks from MaskFormer to mask the images into K-classes, get the text encoding with additional text for saying that there is no object in the class. Pass them through CLIP. 
    - Keeping the background information degrades the overall performance.
    - **Reflection**: Masking can potentially help the large and complex model such as CLIP to perform better by already only highlighting where the model should focus.
15. [Per-Pixel Classification is Not All You Need for Semantic Segmentation](https://arxiv.org/pdf/2107.06278.pdf)
    - MaskFormer model to output the segmentation mask.
    - It is a transformer-based model.
    - Additional transformer decode to the pixel decoder.
    - **Reflection**: We can possibly persue the masking to increase the model performance. MaskFormer which is a transformer-based be better than our TSegmentation which does the segmentation mask based on edges of multiple consecutive frames.
16. [Fully Convolutional Networks for Semantic Segmentation](https://arxiv.org/pdf/1411.4038.pdf)
    - FCNN for segmentation mask may also be suitatble and more effective than the MaskFormer. 
    - FCNN means everything is a CNN which in theory should have less parameters than other models.
    - **Reflection**: before persuing a transformer-based segmentation, we should also consider simpler segmentation such as the one proposed by this paper.
17. [Automated Melanoma Recognition in Dermoscopy Images via Very Deep Residual Networks](https://ieeexplore.ieee.org/document/7792699)
    - Segmentation mask to crop the image before sending it to the CNN network improves the classifcation result.
    - This is a two-stage model.
    - Using deep fully convolutional residual network (FCRN) for skin leison segmentation.
    - Using deep residual network (DRN) for the classification.
    - The segmentation is used to crop the image before passing it to the classification model.
    - **Reflection**: one of the use case support the usefulness of segmentation mask in improving classification.
18. [The effects of skin lesion segmentation on the performance of dermatoscopic image classification](https://www.sciencedirect.com/science/article/pii/S0169260720315583)
    - Using segmentation for dematoscopic image classification. 
    -Segmentation also need to consider whether to include the background information or not (in this paper remove degrade the classificaion performance because of the skin, I think) 
    - Segmentation masks did not significantly improve the malignant melanoma (MM) classification performance in any scenario.
    - **Reflection**: another study contradicting the result of the Reference 17. Hence, to improve the performance of the classification by including segmentation mask may be experimental and depending on the dataset.
19. [Is synthetic data from generative models ready for image recognition?](https://arxiv.org/abs/2210.07574)
    - "It is worth noting that for 1), we only studied the zero-shot and few-shot settings because the positive impact of synthetic data diminishes as more shots are present."
    - Synthetic data are useful for model pre-training for transfer learning. It favors ViT backbone unsupervised learning.
    - In the downstream aware setting, synthetic data deliver close performace to real image pretraining. And with more synthetic data, it could out perform.
    - **Reflection**:
        1. We could use this to first train the model on the synthetic data and use it to test on the real data.
        2. With this we could also have a mask ground truth, which is great for training the segmentation mask model.
20. [Simultaneous Segmentation and Classification of Pressure Injury Image Data Using Mask-R-CNN](https://www.ncbi.nlm.nih.gov/pmc/articles/PMC9911250/)
    - Mask-RCNN - classification and segmentation simultaneously on the medical data.
    - It is what Mask-RCNN does, they just used it on their medical dataset...
21. [Vision Transformers for Remote Sensing Image Classification](https://www.mdpi.com/2072-4292/13/3/516)
    - For remote sensing (aerial images / satellite images).
    - Apart from ViT, advanced data augmentation technique was used, namely CutOut, MixUp and CutMix. 
    - Another contribution they did is the network compression. This paper used model pruning on the transformer to remove the encoder's layers 
    - **Reflection**: It might be interesting to to advanced data augmentation technique if our model turns out not be to robust enough. Also, model pruning on the transformer architecture might prove to be useful if we will have to use one.
22. [Rethinking vision transformer through human–object interaction detection](https://www.sciencedirect.com/science/article/pii/S095219762300307X)
    - Addining CLS token for each of the task instead of just a classification.
    - Human box, interaction, and object box explicitly. 
    - This may help model giving a meaningful encoding on each special token. 
    - They are using a transformer for object detection too, so they introduced the object detection related loss to the model.
    - **Reflection**: if we would have to use a transformer model, adding additional token for each task maybe useful. But for classification, only one CLS as the original ViT paper is enough.
23. [Vision Transformer Visualization: What Neurons Tell and How Neurons Behave?](https://arxiv.org/pdf/2210.07646.pdf)
    - Mentioning how are ViTs robust to deversarial attacks, image occlusions, or patch shuffling.
24. [An Image is Worth 16x16 Words: Transformers for Image Recognition at Scale](https://arxiv.org/abs/2010.11929)
    - Vision Transformer divides image into 16x16 patches and use the learnable linear project to encode the patches into vectors. 
    - These vectors were passed through Transformer Encoder which consists of MSA (qkv) and MLP. - This yields same amout of number of input tokens.
    - Adding CLS token, we use that encoded vector throught another MLP head to do the classification.
    - **Reflection**: This would be the first transformer model to try on if we have to try one.
25. [Understanding Neural Networks via Feature Visualization: A survey](https://arxiv.org/pdf/1904.08939.pdf)
    - feature visualization. 
    - starting from a random image, then optimize it to maximize the class activation of the neural network. 
    - This may give the understanding of what the network thinking about the concept of each class.
    - **Reflection**: in additional to CAM localization, this technique might be another viable way for checking qualitative robustness of the model.


# Plan
If the model works well on the real dataset. Then we should check the robustness of the model. 
For our case, we should see if the model has the "natural robustness"
Then, we should evaluation the interpretability of the model using 
1. CAM visualization (this is the simplest and our ResNet-18 is compatible with it)
2. Feature visualization to see what kind of image would maximize the activation of each class

For the CAM, it is useful to also use it at the inference time. Thus, Recipo-CAM (faster way of obtaining CAM) might be useful.

If the model is not robust.
1. If the model is mistaking the tools within the same group but different size, we could add some loss that encourages the model to be more discriminative within the group of toolheads (I have not read any paper about this)
2. We should try different SoTA and ViT (which is the current state of the art of image classification, and it has the self attention vector which implicity suggest the model interpretability).
2.1 If the ViT works, we should test its time and memory requirement. One way to scale down the model is bydoing some pruning or distillation, or using lightweight ViT
2.2 If any of the model works, then we should investigate the model explainability.

if time permits CLIP is interesting 