# Text Classification

文本分类是给给定文本分配一个标签或者一个类别。


## 分类变体

### 1. Natural Language Inference（NLI）

在自然语言推理（NLI）任务中，模型需判断两段给定文本之间的关系。具体来说，模型会接收一个前提和一个假设，并输出一个类别，该类别可能为以下几种之一：
- 蕴含：前提-假设，为真
- 矛盾：表示假设为假
- 中立：前提-假设之间关联性不大

该任务的基准数据集是GLUE（通用语言理解评估）。自然语言推理模型有多种变体，例如多领域自然语言推理、问答自然语言推理和威诺格拉德自然语言推理。

### 2. Multi-Genre NLI(MNLI)


MNLI 用于通用自然语言推理。这里有一些示例：
```text
Example 1:
    Premise: A man inspects the uniform of a figure in some East Asian country.
    Hypothesis: The man is sleeping.
    Label: Contradiction

Example 2:
    Premise: Soccer game with multiple males playing.
    Hypothesis: Some men are playing a sport.
    Label: Entailment

```

### 3. Question Natural Language Inference(QNLI)

**当前文档是否能够满足回答问题？**

QNLI是一项判断某个问题的答案是否能在给定文档中找到的任务。如果能找到答案，标签为“蕴含”；如果找不到答案，标签为“不蕴含”。


```text
Question: What percentage of marine life died during the extinction?
Sentence: It is also known as the “Great Dying” because it is considered the largest mass extinction in the Earth’s history.
Label: not entailment

Question: Who was the London Weekend Television’s Managing Director?
Sentence: The managing director of London Weekend Television (LWT), Greg Dyke, met with the representatives of the "big five" football clubs in England in 1990.
Label: entailment

```

### 4. Quora Question Pairs (Quora问题对)

Quora Question Pairs模型用于评估两个给定问题是否互为复述句。该模型接收两个问题并返回一个二进制值，其中0对应“非复述句”，1对应“复述句”。该基准数据集是GLUE基准测试中的Quora Question Pairs。该数据集包含问题对及其对应的标签。

**判断两个问题是否重复？**

### 5. Grammatical Correctness 语法正确性

语言可接受性是评估句子语法可接受性的任务。该任务的类别分为“可接受”和“不可接受”。此任务所使用的基准数据集为语言可接受性语料库（CoLA）。该数据集包含文本及其对应的标签。

