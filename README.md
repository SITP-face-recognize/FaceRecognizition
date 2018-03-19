# opecv-practice
just for practice demo


# Face Recongnizition
>>## 1.VS环境配置
使用编译好的install文件夹替代原先自己的opencv文件夹

### 1.VC++目录，包含目录，库目录设置，debug X64

### 2.链接器输入加入以下，并将对应的dll移入C:\windows\system32以及c:\windows\wow64（后缀d为debug版本，没有为realse版本）
```
opencv_aruco341d.lib
opencv_bgsegm341d.lib
opencv_bioinspired341d.lib
opencv_calib3d341d.lib
opencv_ccalib341d.lib
opencv_core341d.lib
opencv_datasets341d.lib
opencv_dnn341d.lib
opencv_dnn_objdetect341d.lib
opencv_dpm341d.lib
opencv_face341d.lib
opencv_features2d341d.lib
opencv_flann341d.lib
opencv_fuzzy341d.lib
opencv_hfs341d.lib
opencv_highgui341d.lib
opencv_imgcodecs341d.lib
opencv_imgproc341d.lib
opencv_img_hash341d.lib
opencv_line_descriptor341d.lib
opencv_ml341d.lib
opencv_objdetect341d.lib
opencv_optflow341d.lib
opencv_phase_unwrapping341d.lib
opencv_photo341d.lib
opencv_plot341d.lib
opencv_reg341d.lib
opencv_rgbd341d.lib
opencv_saliency341d.lib
opencv_shape341d.lib
opencv_stereo341d.lib
opencv_stitching341d.lib
opencv_structured_light341d.lib
opencv_superres341d.lib
opencv_surface_matching341d.lib
opencv_text341d.lib
opencv_tracking341d.lib
opencv_video341d.lib
opencv_videoio341d.lib
opencv_videostab341d.lib
opencv_xfeatures2d341d.lib
opencv_ximgproc341d.lib
opencv_xobjdetect341d.lib
opencv_xphoto341d.lib
```

### 3. 各模块功能及使用顺序（使用时自行更改相应的path）
__Camera__ ：简易照相机，素材采集过程

__PreProcessing__ ：预处理过程，将素材中人脸检测出来获取ROI（region of interst) 并改变大小为92*112，一人一个文件夹

__CSVgenerator__ : 对准备好的预处理过的素材 生成CSV文件(a.txt)，实际就是 
```
图片路径;标签（一人一个标签）
```

__ModelTraining__ : 对做好的模型做训练，生成训练集xml文件，共三种方法，生成三个文件

__FaceReconizition__ : 人脸识别
