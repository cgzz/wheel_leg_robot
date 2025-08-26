// /assets/js/services/websocket.js
import { state, domElements } from '../config.js';
import { setStatus, appendLog } from '../ui.js';

let ws = null;
let reconnectTimer = null;
let telemetryCallback = null;
let uiConfigCallback = null;
let pidParamsCallback = null;

/**
 * 发送 WebSocket 消息 (JSON)
 * @param {object} obj 
 */
export function sendWebSocketMessage(obj) {
  if (ws && ws.readyState === WebSocket.OPEN) {
    ws.send(JSON.stringify(obj));
  }
}

/**
 * 处理收到的 WebSocket 消息
 * @param {MessageEvent} event 
 */
function handleMessage(event) {
  let msg = null;
  try {
    msg = JSON.parse(event.data);
  } catch (e) {
    console.error('Failed to parse WebSocket message:', event.data);
    return;
  }

  if (!msg || !msg.type) return;

  switch (msg.type) {
    case 'telemetry':
      if (telemetryCallback) telemetryCallback(msg);
      break;
    case 'ui_config':
      if (uiConfigCallback) uiConfigCallback(msg);
      break;
    case 'pid':
      if (pidParamsCallback) pidParamsCallback(msg);
      break;
    case 'info':
      if (msg.text) appendLog(`[INFO] ${msg.text}`);
      break;
    default:
      // 未知消息类型
      break;
  }
}

/**
 * 尝试重新连接 WebSocket
 */
function reconnect() {
  clearTimeout(reconnectTimer);
  reconnectTimer = setTimeout(connectWebSocket, 1000);
}

/**
 * 连接到 WebSocket 服务器
 * @param {object} callbacks - 回调函数集合
 * @param {function} callbacks.onTelemetry - 遥测数据回调
 * @param {function} callbacks.onUiConfig - UI配置数据回调
 * @param {function} callbacks.onPidParams - PID参数数据回调
 */
export function connectWebSocket(callbacks = {}) {
  if (callbacks.onTelemetry) telemetryCallback = callbacks.onTelemetry;
  if (callbacks.onUiConfig) uiConfigCallback = callbacks.onUiConfig;
  if (callbacks.onPidParams) pidParamsCallback = callbacks.onPidParams;

  const protocol = (location.protocol === 'http:') ? 'ws://' : 'wss://';
  const url = `${protocol}${location.host}/ws`;

  setStatus('connecting…');

  try {
    ws = new WebSocket(url);

    ws.onopen = () => {
      state.connected = true;
      setStatus('connected');
      sendWebSocketMessage({ type: 'get_pid' });
      appendLog('[SEND] get_pid');
    };

    ws.onclose = () => {
      state.connected = false;
      setStatus('reconnecting…');
      reconnect();
    };

    ws.onerror = () => {
      state.connected = false;
      setStatus('error');
      // onerror 也会触发 onclose，所以重连逻辑在 onclose 中统一处理
    };

    ws.onmessage = handleMessage;

  } catch (e) {
    console.error('WebSocket connection failed:', e);
    reconnect();
  }
}

/**
 * 通过 HTTP API 同步初始状态
 */
export async function syncInitialState() {
  try {
    const response = await fetch('/api/state');
    if (!response.ok) throw new Error(`HTTP error! status: ${response.status}`);
    const s = await response.json();

    if (typeof s.hz === 'number') domElements.rateHzInput.value = s.hz;
    if (typeof s.running === 'boolean') domElements.runSwitch.checked = !!s.running;
    if (typeof s.chart_enable === 'boolean') {
        domElements.chartSwitch.checked = !!s.chart_enable;
        state.chartsOn = !!s.chart_enable;
    }
    appendLog(`[INIT] /api/state ok -> running=${s.running}, hz=${s.hz}`);
  } catch (e) {
    appendLog(`[INIT] /api/state fail: ${e.message}`);
  }
}