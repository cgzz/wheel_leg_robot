// /js/main.js

// 核心模块
import { initApi } from './core/api.js';
import { initWebSocket } from './core/websocket.js';

// UI 模块
import { initDomElements } from './ui/dom-elements.js';
import { initLogger } from './ui/logger.js';
import { initIndicators } from './ui/indicators.js';
import { initToolbar } from './ui/toolbar.js';
import { initPidControls } from './ui/pid.js';
import { initTestMode } from './ui/test-mode.js';
import { initCharts } from './ui/charts.js';
import { initAttitude3D } from './ui/attitude3d.js';
import { initJoystick } from './ui/joystick.js';

/**
 * 应用主函数，负责初始化所有模块。
 * 这是整个前端逻辑的起点。
 */
function main() {
    // 1. 优先初始化DOM元素缓存，确保其他UI模块都能正确访问元素
    initDomElements();

    // 2. 初始化各个功能UI模块
    initLogger();       // 事件日志和状态显示
    initIndicators();   // 摔倒和电压指示灯
    initToolbar();      // 顶部控制开关
    initPidControls();  // PID控制面板
    initTestMode();     // 测试模式面板
    initCharts();       // 图表
    initAttitude3D();   // 3D姿态显示
    initJoystick();     // 遥控杆

    // 3. 从后端获取初始状态，同步UI
    initApi();

    // 4. 建立WebSocket通信连接
    initWebSocket();
}

// 当HTML文档加载完毕后，启动应用主函数
document.addEventListener('DOMContentLoaded', main);