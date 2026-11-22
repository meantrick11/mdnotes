# D2L 前置 03｜Notation 速查

> **原书：** Alexander J. Smola, *Dive into Deep Learning*, PyTorch Edition，PDF 标注日期 2026-11-11。  
> **范围：** *Notation*，书页 15–20（PDF 第 55–60 页）。  
> **原书：** [Smola - Dive into Deep Learning.pdf](books/Smola%20-%20Dive%20into%20Deep%20Learning.pdf)

原书强调：符号表里的 **“a” 表示一类符号的习惯写法**，如小写斜体常表示标量；**“the” 则定义某个固定符号**，如 $\mathbb R$ 指实数集。同一个字母可因字体、下标或参数形式不同而有不同含义，读公式时要连同这些标记一起看。

## 数值对象与集合

| 记号 | 含义 | 易混点 |
|---|---|---|
| $x,\ \mathbf x,\ \mathbf X,\ \mathsf X$ | 标量、向量、矩阵、超过两个轴的张量 | 字体表示对象类型；PDF 提取文本有时会丢失字体差别 |
| $x_i,\ X_{ij}$ | 向量第 $i$ 个元素、矩阵第 $i$ 行第 $j$ 列元素 | $i,j$ 是索引 |
| $\mathbf I,\ \mathbf 0,\ \mathbf 1$ | 单位矩阵、全零向量、全一向量 | 维度由上下文确定 |
| $\mathcal X,\ |\mathcal X|$ | 集合、集合元素个数 | 竖线在此表示基数 |
| $\mathbb R^n,\ \mathbb R^{a\times b}$ | $n$ 维实向量空间、$a$ 行 $b$ 列实矩阵空间 | 行数写在前面 |
| $\mathcal A\cup\mathcal B,\ \mathcal A\cap\mathcal B,\ \mathcal A\setminus\mathcal B$ | 并、交、差 | 差集有方向 |

## 函数、线性代数与微积分

| 记号 | 含义 | 使用时注意 |
|---|---|---|
| $\log x,\ \log_2 x,\ \exp x$ | 自然对数、以 2 为底的对数、指数 | 未写底数的 `log` 在本书指自然对数 |
| $\sigma(x)=1/(1+e^{-x})$ | sigmoid 函数 | $\sigma_X$ 可表示标准差，$\sigma_i(\mathbf X)$ 可表示奇异值 |
| $\operatorname{softmax}(\mathbf o)_i=e^{o_i}/\sum_j e^{o_j}$ | 将分数向量转为各分量非负、总和为 1 的向量 | 下标 $i$ 指输出分量 |
| $\mathbf 1(\text{条件})$ | 指示函数，条件真为 1，否则为 0 | 与全一向量 $\mathbf 1$ 区分：这里有参数 |
| $\arg\min_x f(x),\ \arg\max_x f(x)$ | 使函数取最小、最大值的自变量 | 返回位置/自变量，不是函数值 |
| $a\propto b,\ a\stackrel{\mathrm{def}}{=}b$ | 成比例、按定义等于 | 比例因子不依赖当前关心的变量 |
| $\sum,\ \prod,\ O(n^2)$ | 求和、连乘、至多相差常数因子的渐近上界 | $O(n^2)$ 是增长量级，不是精确操作次数 |
| $\mathbf X^\top,\ \mathbf X^{-1},\ \mathbf X^+$ | 转置、逆、Moore–Penrose 伪逆 | 并非每个矩阵都有普通逆 |
| $\langle\mathbf x,\mathbf y\rangle,\ \mathbf x\odot\mathbf y$ | 内积、逐元素乘积 | 内积产出标量；逐元素乘积保留相应形状 |
| $\|\mathbf x\|_p,\ \|\mathbf X\|_F,\ \|\mathbf X\|_2$ | 向量 $\ell_p$ 范数、矩阵 Frobenius 范数、矩阵谱范数 | 矩阵 $\|\mathbf X\|_2$ 是最大奇异值 |
| $\operatorname{diag}(\mathbf x),\ \operatorname{rank}\mathbf X,\ \operatorname{tr}\mathbf X,\ \det\mathbf X$ | 对角矩阵、秩、迹、行列式 | 按矩阵性质使用，不要把迹与行列式混同 |
| $\lambda_i,\ \sigma_i(\mathbf X),\ \kappa(\mathbf X),\ \rho(\mathbf X)$ | 特征值、奇异值、条件数、谱半径 | $\kappa$ 用最大与最小非零奇异值之比定义；$\rho$ 是特征值模的最大值 |
| $\mathbf X\succeq0,\ \mathbf X\succ0$ | 对称矩阵半正定、正定 | 严格正定要求所有非零向量的二次型为正 |
| $\nabla_{\mathbf x}y,\ \nabla^2 f$ | 对向量 $\mathbf x$ 的梯度、Hessian | 前者是一阶导数组，后者是二阶偏导数组 |

这些矩阵记号在后续数学推导中再结合具体问题学习；此处先记住 **对象类型、索引、运算结果**，不要把不同对象上的相似竖线或下标混为一谈。

## 概率、数据与训练

| 记号 | 含义 | 使用时注意 |
|---|---|---|
| $X\sim P,\ P(X=x),\ p(x)$ | 随机变量服从分布、事件概率、概率密度 | 离散概率与连续密度不是同一数值对象 |
| $\mathbb E[X],\ \operatorname{Var}(X),\ \operatorname{Cov}(X,Y)$ | 期望、方差、协方差 | $\operatorname{Var}(X)=\sigma_X^2$ |
| $X\perp Y,\ X\perp Y\mid Z$ | 独立、给定 $Z$ 时条件独立 | 条件独立不自动推出无条件独立 |
| $\mathcal N(\mu,\sigma^2)$ | 均值 $\mu$、方差 $\sigma^2$ 的正态分布 | 第二个参数是方差，不是标准差 |
| $H(X),\ D_{\mathrm{KL}}(P\|Q)$ | 熵、从 $P$ 到 $Q$ 的 KL 散度 | KL 的两侧顺序有意义 |
| $n,d,\ \mathbf x^{(i)},y^{(i)}$ | 样本数、输入特征数、第 $i$ 个样本的特征与标签 | 上标 $(i)$ 是样本索引，不是幂 |
| $\mathbf X\in\mathbb R^{n\times d},\ \mathcal B$ | 一行一个样本的设计矩阵、小批量样本集 | $|\mathcal B|$ 是批量大小 |
| $\hat y,\ \mathbf w,b,\ \theta$ | 预测值、权重与偏置、模型参数 | 带帽的 $\hat y$ 与真实 $y$ 区分 |
| $l,\ell,\ L,\ \eta,\lambda$ | 单样本损失、批量/数据集平均损失、学习率、权重衰减惩罚系数 | $\lambda$ 的含义须结合所在章节核对 |

## 后续章节的专用符号索引

原书在本节还预列了以下符号；这里只记录“看到它时去哪里查”，不提前推导后续机制。

| 领域 | 本节出现的代表记号 | 后续位置 |
|---|---|---|
| 注意力与 Transformer | $\mathbf q,\mathbf k,\mathbf v$；矩阵 $\mathbf Q,\mathbf K,\mathbf V$；注意力权重 $\alpha$ | 第 10–11 章 |
| 状态空间模型 | 状态矩阵 $\mathbf A$、输入映射 $\mathbf B$、读出 $\mathbf C$、跳连项 $D$ | 第 12 章 |
| 强化学习 | 状态 $s_t$、动作 $a_t$、奖励 $r_t$、策略 $\pi$、折扣因子 $\gamma$ | 第 14–15 章 |
| GAN | 数据密度 $p$、生成分布密度 $q$、生成器 $G$、判别分数 $D$ | 第 16 章 |
| 扩散模型 | 干净数据 $\mathbf x_0$、噪声 $\epsilon$、噪声时刻样本 $\mathbf x_t$ | 第 17 章 |

这些章节中的符号可能与前面常用记号重名；**局部章节定义和下标优先于字母的直觉含义**。例如 $\sigma(x)$ 是 sigmoid，$\sigma_X$ 是标准差，$\sigma_t$ 在扩散章节可指噪声系数。

## 最小辨析

- $\arg\min_w L(w)$ 是使损失最小的参数；$\min_w L(w)$ 是最小损失值。
- $\mathbf x^{(i)}$ 是第 $i$ 个样本；$x_i$ 是同一向量的第 $i$ 个元素。
- $\|\mathbf X\|_F$ 汇总矩阵所有元素的平方；$\|\mathbf X\|_2$ 衡量最大奇异值，两者一般不同。
