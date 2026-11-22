# PyTorch 简介笔记 
## 一、什么是 PyTorch
PyTorch 是 Meta（原Facebook）开源的**基于Python的深度学习框架**，底层核心是张量运算 + 自动微分引擎，主打动态计算图，是科研、工业界AI模型开发最主流框架之一。 > 定位：**底层深度学习引擎**，帮我们封装张量运算、自动求导、反向传播、GPU加速，开发者专注定义网络前向逻辑，不用手写链式求导、矩阵加速代码。
## 二、核心特点 
1. **动态计算图（Dynamic Graph）** 计算图在代码运行时实时构建，不是提前静态编译。可以写if、for分支，调试友好，print、断点直接看张量值，科研实验非常灵活。 > 对比TensorFlow早期静态图：先搭图再喂数据，调试麻烦。 
2. **自动微分 Autograd** 核心模块，张量带`requires_grad`标记，记录计算历史；调用`.backward()`自动链式求导，得到参数梯度，支撑反向传播。
3. **Pythonic，简单易用** 语法贴近原生Python，代码可读性高，上手门槛低，和numpy风格很像，张量可以无缝迁移GPU。 
4. **GPU加速友好** 一行代码把张量/模型移到cuda：`model.cuda()` / `model.to("cuda")`，卷积、矩阵运算底层调用CUDA加速。
5. **生态完善** - torch.nn：网络层封装（Conv2d、Linear、ReLU等） - torch.nn.functional：函数式层（无参数操作，max_pool2d、relu） - torch.optim：优化器（SGD、Adam等，用来更新参数） - torch.utils.data：数据集、数据加载工具 - 上层生态：TorchVision、TorchAudio、TorchText、HuggingFace、Lightning

## 三、核心组件 
| 模块 | 作用 |
| ---- | ---- | 
| torch.Tensor | 基础数据单元，类似numpy数组，支持GPU、自动求导 |
| torch.autograd | 自动微分引擎，梯度计算、反向传播核心 |
| torch.nn | 神经网络模块，封装层、损失函数，`nn.Module`是所有网络基类 |
| torch.optim | 参数优化器，根据梯度更新权重 | 
| torch.utils.data | 数据集封装、Dataloader批量加载数据 | 
## 四、基础工作流程（以LeNet为例）
1. 继承`nn.Module`自定义网络，`__init__`里声明可学习层（conv、linear）
2. 重写`forward`函数：定义**前向传播路径**（数据流动顺序） > ✅ 重点：PyTorch只需要你写前向逻辑，反向传播由autograd自动生成
3. 准备数据、损失函数、优化器 
4. 训练循环： 前向计算输出 → 计算loss → `loss.backward()`自动求梯度 → 优化器step更新参数 
5. 验证、推理
## 五、适用场景 
✅ 科研实验、论文复现（动态图方便试错）
✅ 计算机视觉、NLP、大模型、Agent、RAG模型开发 
✅ 工业落地部署（TorchScript、ONNX导出）
❌ 不适合：零基础只想要一行代码训练、零代码建模（这类是高层封装，比如transformers、sklearn）
## 六、一句话总结
PyTorch 是**自动微分驱动的张量计算框架**，帮你搞定梯度、反向传播、硬件加速；开发者只需要定义数据怎么走（前向），适合从底层搭建神经网络。 ## 七、补充区分（你刚才的疑问） - PyTorch：底层框架，管张量、求导、网络搭建、参数更新 - HuggingFace Transformers：基于PyTorch/TensorFlow的高层库，封装好预训练大模型，直接调用 如果你需要，我可以再导出成适合PPT的精简版，或者markdown版本。