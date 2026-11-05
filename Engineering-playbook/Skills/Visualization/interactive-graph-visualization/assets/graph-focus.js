/* 原生 SVG 图聚焦组件；无框架和业务领域依赖。 */
(function (root) {
  'use strict';
  var SVG_NS = 'http://www.w3.org/2000/svg';
  var MODES = new Set(['direct', 'upstream', 'downstream', 'all']);

  function computeFocus(nodes, edges, selectedNodeId, mode) {
    var ids = new Set(nodes.map(function (node) { return node.id; }));
    var highlightedNodeIds = new Set();
    var highlightedEdgeIds = new Set();
    if (!ids.has(selectedNodeId)) return { highlightedNodeIds: highlightedNodeIds, highlightedEdgeIds: highlightedEdgeIds };
    var scope = MODES.has(mode) ? mode : 'direct';
    var adjacency = new Map();
    ids.forEach(function (id) { adjacency.set(id, []); });
    edges.forEach(function (edge) {
      if (!ids.has(edge.source) || !ids.has(edge.target)) return;
      adjacency.get(edge.source).push({ edge: edge, next: edge.target, direction: 'downstream' });
      adjacency.get(edge.target).push({ edge: edge, next: edge.source, direction: 'upstream' });
    });
    var visited = new Set([selectedNodeId]);
    var queue = [selectedNodeId];
    highlightedNodeIds.add(selectedNodeId);
    for (var index = 0; index < queue.length; index += 1) {
      adjacency.get(queue[index]).forEach(function (link) {
        if (scope === 'upstream' && link.direction !== 'upstream') return;
        if (scope === 'downstream' && link.direction !== 'downstream') return;
        highlightedEdgeIds.add(link.edge.id);
        highlightedNodeIds.add(link.next);
        if (scope !== 'direct' && !visited.has(link.next)) {
          visited.add(link.next);
          queue.push(link.next);
        }
      });
      if (scope === 'direct') break;
    }
    return { highlightedNodeIds: highlightedNodeIds, highlightedEdgeIds: highlightedEdgeIds };
  }

  function createGraphFocus(options) {
    var svg = options.svg;
    var graph = options.graph;
    if (!svg || !graph || !Array.isArray(graph.nodes) || !Array.isArray(graph.edges)) {
      throw new TypeError('svg、graph.nodes 和 graph.edges 是必需的');
    }
    var nodeElements = new Map();
    var edgeElements = new Map();
    svg.querySelectorAll('[data-graph-node-id]').forEach(function (element) {
      nodeElements.set(element.getAttribute('data-graph-node-id'), element);
    });
    svg.querySelectorAll('path[data-graph-edge-id]').forEach(function (element) {
      edgeElements.set(element.getAttribute('data-graph-edge-id'), element);
    });
    var overlay = document.createElementNS(SVG_NS, 'g');
    overlay.setAttribute('class', 'igv-flow-overlay');
    overlay.setAttribute('aria-hidden', 'true');
    svg.appendChild(overlay);
    var selectedNodeId = null;
    var mode = MODES.has(options.mode) ? options.mode : 'direct';

    function render() {
      var focus = computeFocus(graph.nodes, graph.edges, selectedNodeId, mode);
      nodeElements.forEach(function (element, id) {
        element.classList.toggle('is-igv-selected', id === selectedNodeId);
        element.classList.toggle('is-igv-related', selectedNodeId !== null && focus.highlightedNodeIds.has(id));
        element.classList.toggle('is-igv-dimmed', selectedNodeId !== null && !focus.highlightedNodeIds.has(id));
        element.setAttribute('aria-pressed', String(id === selectedNodeId));
      });
      edgeElements.forEach(function (element, id) {
        element.classList.toggle('is-igv-related', selectedNodeId !== null && focus.highlightedEdgeIds.has(id));
        element.classList.toggle('is-igv-dimmed', selectedNodeId !== null && !focus.highlightedEdgeIds.has(id));
      });
      overlay.replaceChildren();
      if (selectedNodeId === null) return;
      focus.highlightedEdgeIds.forEach(function (id, index) {
        var original = edgeElements.get(id);
        if (!original || !original.getAttribute('d')) return;
        var length;
        try { length = original.getTotalLength(); } catch (_) { return; }
        if (!Number.isFinite(length) || length <= 0) return;
        var segment = Math.max(5, Math.min(18, length * 0.22));
        var path = document.createElementNS(SVG_NS, 'path');
        path.setAttribute('d', original.getAttribute('d'));
        path.setAttribute('class', 'igv-flow');
        path.setAttribute('stroke-dasharray', segment + ' ' + length);
        path.style.setProperty('--igv-period', String(length + segment));
        path.style.animationDelay = (-index * 0.16) + 's';
        overlay.appendChild(path);
      });
    }
    function select(id) {
      if (!graph.nodes.some(function (node) { return node.id === id; })) return;
      selectedNodeId = selectedNodeId === id ? null : id;
      render();
    }
    function clear() { selectedNodeId = null; render(); }
    function setMode(next) {
      if (!MODES.has(next)) throw new TypeError('未知关联范围: ' + next);
      mode = next;
      render();
    }
    function click(event) {
      var node = event.target.closest('[data-graph-node-id]');
      if (node && svg.contains(node)) select(node.getAttribute('data-graph-node-id'));
      else clear();
    }
    function keydown(event) {
      var node = event.target.closest('[data-graph-node-id]');
      if (node && (event.key === 'Enter' || event.key === ' ')) {
        event.preventDefault();
        select(node.getAttribute('data-graph-node-id'));
      } else if (event.key === 'Escape' && selectedNodeId !== null) clear();
    }
    if (options.bindEvents !== false) {
      svg.addEventListener('click', click);
      svg.addEventListener('keydown', keydown);
    }
    return {
      select: select,
      clear: clear,
      setMode: setMode,
      getState: function () { return { selectedNodeId: selectedNodeId, mode: mode }; },
      destroy: function () {
        svg.removeEventListener('click', click);
        svg.removeEventListener('keydown', keydown);
        clear();
        overlay.remove();
      }
    };
  }

  root.InteractiveGraphVisualization = { computeFocus: computeFocus, createGraphFocus: createGraphFocus };
})(window);
