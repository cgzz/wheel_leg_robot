// /js/state.js

/**
 * 全局状态管理对象。
 * 用于存储跨模块共享的状态，例如WebSocket连接状态、图表推送开关等。
 * 这样做可以避免状态分散在各个模块中，便于集中管理和调试。
 */
export const state = {
  // WebSocket 是否已连接
  connected: false,

  // 图表推送功能是否开启
  chartsOn: true,
};