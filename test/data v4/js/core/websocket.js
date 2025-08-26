// /js/core/websocket.js

import { state } from '../state.js';
import { appendLog, setStatus } from '../ui/logger.js';
import { updateCharts, applyChartConfig } from '../ui/charts.js';
import { updateAttitude } from '../ui/attitude3d.js';
import { fillPidToUI, applySliderConfig } from '../ui/pid.js';
import { updateIndicators } from '../ui/indicators.js';

let ws = null;
let reconnectTimer = null;

/**
 * 导出的函数，用于发送JSON对象到WebSocket服务器。
 * @param {object} obj - 需要发送的JavaScript对象。
 */
export function wsSend(obj) {
  if (ws && ws.readyState === WebSocket.OPEN) {
    ws.send(JSON.stringify(obj));
  }
}

/**
 * 处理遥测数据 (telemetry) 消息。
 * @param {object} msg - 从WebSocket接收到的消息对象。
 */
function handleTelemetry(msg) {
  // 更新3D姿态
  if (Number.isFinite(msg.pitch) && Number.isFinite(msg.roll) && Number.isFinite(msg.yaw)) {
    updateAttitude(msg.pitch, msg.roll, msg.yaw);
  }
  
  // 如果图表开启，则更新图表数据
  if (state.chartsOn && Array.isArray(msg.d) && msg.d.length >= 9) {
    updateCharts(...msg.d);
  }
  
  // 更新指示灯（摔倒和电压）
  updateIndicators(msg.fallen, msg.voltage);
}

/**
 * 处理UI配置 (ui_config) 消息。
 * @param {object} msg - 从WebSocket接收到的消息对象。
 */
function handleUiConfig(msg) {
  if (Array.isArray(msg.charts)) {
    applyChartConfig(msg.charts);
  }
  if (Array.isArray(msg.sliders)) {
    applySliderConfig(msg.sliders);
  }
}

/**
 * 处理PID参数 (pid) 消息。
 * @param {object} msg - 从WebSocket接收到的消息对象。
 */
function handlePid(msg) {
  if (msg.param) {
    fillPidToUI(msg.param);
  }
}

// 使用一个映射表来处理不同类型的消息，使代码更清晰
const messageHandlers = {
  'telemetry': handleTelemetry,
  'ui_config': handleUiConfig,
  'pid': handlePid,
  'info': (msg) => appendLog(`[INFO] ${msg.text}`),
};

/**
 * WebSocket onmessage 事件的回调函数。
 * @param {MessageEvent} event - WebSocket消息事件。
 */
function onMessage(event) {
  let msg = null;
  try {
    msg = JSON.parse(event.data);
  } catch (e) {
    console.error("Failed to parse WebSocket message:", event.data);
    return; // 忽略非JSON格式的消息
  }

  // 根据消息类型调用对应的处理器
  if (msg && msg.type && messageHandlers[msg.type]) {
    messageHandlers[msg.type](msg);
  }
}

/**
 * 建立WebSocket连接。
 */
function connect() {
  const protocol = (location.protocol === 'http:') ? 'ws://' : 'wss://';
  const url = `${protocol}${location.host}/ws`;

  ws = new WebSocket(url);

  ws.onopen = () => {
    state.connected = true;
    setStatus('connected');
    appendLog('[WS] Connection opened.');
    wsSend({ type: 'get_pid' }); // 连接成功后请求一次PID参数
  };

  ws.onclose = () => {
    state.connected = false;
    setStatus('reconnecting…');
    clearTimeout(reconnectTimer);
    reconnectTimer = setTimeout(connect, 2000); // 2秒后尝试重连
  };

  ws.onerror = (error) => {
    state.connected = false;
    setStatus('error');
    console.error('WebSocket Error:', error);
    ws.close(); // 这会触发 onclose 事件，从而启动重连逻辑
  };

  ws.onmessage = onMessage;
}

/**
 * 初始化WebSocket模块。
 */
export function initWebSocket() {
  connect();
}