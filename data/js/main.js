// /assets/js/main.js
import { state } from './config.js';
import { initUI, updateFallIndicator, updateVoltageIndicator, logLine } from './ui.js';
import { initCharts, feedChartsData, applyChartConfig } from './modules/charts.js';
import { init3D, setAttitude } from './modules/robot3D.js';
import { initPID, fillPidToUI, applySliderConfig } from './modules/pid.js';
import { initJoystick } from './modules/joystick.js';
import { initTestMode } from './modules/testMode.js';
import { connectWebSocket, syncInitialState } from './services/websocket.js';

/**
 * 主初始化函数
 */
async function main() {
  logLine('initializing...');
  
  // 初始化各个模块
  initUI();
  initCharts();
  initPID();
  initJoystick();
  init3D();
  initTestMode();

  // 启动时将指示灯置为初始状态
  updateFallIndicator(null);
  updateVoltageIndicator(null);
  
  // 从后端同步初始状态 (HTTP)
  await syncInitialState();
  
  // 建立 WebSocket 连接
  connectWebSocket({
    onTelemetry: (msg) => {
      // 姿态更新
      if (typeof msg.pitch !== 'undefined') {
        setAttitude(msg.pitch, msg.roll, msg.yaw);
      }
      
      // 图表和指示灯更新 (仅在图表开启时)
      if (state.chartsOn) {
        if (Array.isArray(msg.d) && msg.d.length >= 9) {
          feedChartsData(...msg.d);
        }
        if (typeof msg.fallen !== 'undefined') {
          updateFallIndicator(msg.fallen);
        }
        if (typeof msg.voltage !== 'undefined') {
            updateVoltageIndicator(msg.voltage);
        }
      }
    },
    onUiConfig: (msg) => {
      if (msg.charts) applyChartConfig(msg.charts);
      if (msg.sliders) applySliderConfig(msg.sliders);
    },
    onPidParams: (msg) => {
      if (msg.param) fillPidToUI(msg.param);
    }
  });

  logLine('ready');
}

// DOM加载完毕后执行主函数
window.addEventListener('DOMContentLoaded', main);