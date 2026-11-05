# 图数据约定

## 最小结构

```js
const graph = {
  nodes: [
    { id: 'input', label: '输入' },
    { id: 'process', label: '处理' },
    { id: 'output', label: '输出' }
  ],
  edges: [
    { id: 'e1', source: 'input', target: 'process' },
    { id: 'e2', source: 'process', target: 'output' }
  ]
};
```

节点 ID、边 ID 各自唯一且稳定。每条边的 `source`、`target` 引用已存在的节点。并行边使用不同 ID；自环可以保留。标签、类型、分组、说明、坐标和路径可按场景扩展。

## 关系范围

- **Direct**：当前节点及直接相连的节点、边。
- **Upstream**：沿入边反向遍历的全部可达节点、边。
- **Downstream**：沿出边正向遍历的全部可达节点、边。
- **All**：两方向可达的全部节点、边；显示时仍保留各边自身的方向。

状态分成 `selectedNodeId`、`highlightedNodeIds`、`highlightedEdgeIds`。遍历时以节点 ID 维护 `visited`，边用 ID 去重，环路和重复边不会导致无限循环。

## 原生 SVG 接入

```html
<link rel="stylesheet" href="graph-focus.css">
<svg id="graph" viewBox="0 0 600 240">
  <path data-graph-edge-id="e1" d="M 120 120 L 280 120" />
  <g data-graph-node-id="input" tabindex="0" role="button">...</g>
  <g data-graph-node-id="process" tabindex="0" role="button">...</g>
</svg>
<script src="graph-focus.js"></script>
<script>
  const focus = InteractiveGraphVisualization.createGraphFocus({
    svg: document.querySelector('#graph'), graph
  });
  // focus.setMode('upstream'); focus.select('input'); focus.clear(); focus.destroy();
</script>
```

每条边都应有对应的 SVG `path`，每个节点有对应元素。实际绘制时补齐所有边与节点，并让边的 `d` 从 source 指向 target。流光覆盖层由控制器创建，原边不被替换。导出前从 SVG 克隆中移除 `.igv-flow-overlay`，恢复静态完整图。已有框架可只使用 `computeFocus(nodes, edges, selectedNodeId, mode)`，由框架管理交互与渲染。
