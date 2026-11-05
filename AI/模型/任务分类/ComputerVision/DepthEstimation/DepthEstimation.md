# Depth Estimation（深度估计）

深度估计从图片预测场景中各位置与相机的距离，通常输出深度图。**绝对深度**给出米或英尺等距离；**相对深度**只判断远近顺序。

## 例子

输入一张包含前景物体和远处背景的照片，模型输出深度图：近处区域与远处区域对应不同深度值。

## 应用场景

从二维图像推断三维结构，或估计图中物体的体积与空间关系。

来源：[Hugging Face：Depth Estimation](https://huggingface.co/tasks/depth-estimation)
