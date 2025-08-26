// /js/ui/toolbar.js

import { state } from "../state.js";
import { elements } from "./dom-elements.js";
import { wsSend } from "../core/websocket.js";
import { enableTestMode } from "./test-mode.js";
import { appendLog } from "./logger.js";

/**
 * 处理“设置”遥测频率按钮的点击事件。
 */
function handleSetRate() {
  const hz = parseInt(elements.rateHzInput.value, 10);
  if (hz >= 1 && hz <= 60) {
    wsSend({ type: "telem_hz", hz });
    appendLog(`[SEND] telem_hz: ${hz}Hz`);
  } else {
    alert("请输入1-60之间的有效频率值");
  }
}

/**
 * 处理“运行”开关的状态改变事件。
 */
function handleToggleRun() {
  wsSend({ type: "robot_run", running: elements.runSwitch.checked });
}

/**
 * 处理“图表推送”开关的状态改变事件。
 */
function handleToggleCharts() {
  const isEnabled = elements.chartSwitch.checked;
  state.chartsOn = isEnabled;
  wsSend({ type: "charts_send", on: isEnabled });
}

/**
 * 处理“测试模式”开关的状态改变事件。
 */
function handleToggleTestMode() {
  const isEnabled = elements.testModeSwitch.checked;
  enableTestMode(isEnabled);
}

/**
 * 处理“摔倒检测”开关的状态改变事件。
 */
function handleToggleFallDetect() {
  wsSend({ type: "fall_check", enable: elements.fallDetectSwitch.checked });
}

/**
 * 初始化工具栏模块，为所有控件绑定事件监听器。
 */
export function initToolbar() {
  elements.setRateBtn.addEventListener("click", handleSetRate);
  elements.runSwitch.addEventListener("change", handleToggleRun);
  elements.chartSwitch.addEventListener("change", handleToggleCharts);
  elements.testModeSwitch.addEventListener("change", handleToggleTestMode);
  elements.fallDetectSwitch.addEventListener("change", handleToggleFallDetect);
}

/**
 * 根据从后端获取的初始状态，设置工具栏控件的初始显示状态。
 * @param {object} initialState - 从 /api/state 获取的初始状态对象。
 */
export function setToolbarState(initialState) {
  if (typeof initialState.hz === "number") {
    elements.rateHzInput.value = initialState.hz;
  }
  if (typeof initialState.running === "boolean") {
    elements.runSwitch.checked = initialState.running;
  }
  if (typeof initialState.chart_enable === "boolean") {
    elements.chartSwitch.checked = initialState.chart_enable;
  }
  if (typeof initialState.fallen_enable === "boolean") {
    elements.fallDetectSwitch.checked = initialState.fallen_enable;
  }
}
