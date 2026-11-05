# Image Feature Extraction（图像特征提取）

图像特征提取把图片转换成模型学到的数值表示，供相似度比较或下游模型使用。输出是特征向量，不是图片类别。

## 例子

页面示例把图像映射成多维数值特征；两张都包含猫的照片，其表示通常会比猫照与无猫图片的表示更接近。

## 应用场景

比较图片相似度，或把已训练模型的视觉特征迁移到新任务，例如从汽车识别迁移到卡车识别。

来源：[Hugging Face：Image Feature Extraction](https://huggingface.co/tasks/image-feature-extraction)
