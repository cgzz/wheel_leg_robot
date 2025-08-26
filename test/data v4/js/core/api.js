// /js/core/api.js

import { state } from '../state.js';
import { setToolbarState } from '../ui/toolbar.js';
import { appendLog } from '../ui/logger.js';

/**
 * 初始化API模块，通过fetch请求从后端获取初始状态。
 * 这个函数应该在所有UI模块初始化之后，WebSocket连接之前调用。
 */
export async function initApi() {
  try {
    const response = await fetch('/api/state');
    if (!response.ok) {
      throw new Error(`HTTP error! status: ${response.status}`);
    }
    const initialState = await response.json();

    // 将获取到的初始状态应用到UI
    // 1. 更新顶部工具栏的开关状态
    setToolbarState(initialState);
    
    // 2. 更新全局状态中的图表推送标志
    state.chartsOn = !!initialState.chart_enable;

    // 3. 记录成功日志
    appendLog(`[INIT] /api/state OK: running=${initialState.running}, hz=${initialState.hz}`);
  } catch (error) {
    // 4. 如果失败，记录错误日志
    appendLog(`[INIT] /api/state Fail: ${error.message}`);
  }
}