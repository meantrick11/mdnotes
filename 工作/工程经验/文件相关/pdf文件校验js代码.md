## PDF 里的 JavaScript不是明文文本，而是PDF 标准规范定义的结构化对象，检查逻辑就是：
1. PDF 存储 JS 的固定规则（PDF 1.7 规范）
JS 对象的类型标签：/S /JavaScript
JS 存放位置：
文档根目录：/Catalog -> /Names -> /JavaScript
页面动作：/Page -> /AA /A
表单域 / 链接动作
嵌套字典 / 数组
2. 代码做了什么？
用 PDFBox 加载 PDF，解析成结构化对象树
递归遍历所有字典、数组
查找标记为 /JavaScript 的对象
找到就抛出异常（表示文件不安全）