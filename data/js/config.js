// /assets/js/config.js

/**
 * 全局状态管理
 */
export const state = {
  connected: false,
  chartsOn: false,
  isPageVisible: true,
  joystick: { x: 0, y: 0, a: 0, isDragging: false, lastSendTime: 0 },
  pidParams: {},
  attitudeZero: { roll: 0, yaw: 0 },
  charts: {
    chart1: null,
    chart2: null,
    chart3: null,
  },
  three: {
    scene: null,
    camera: null,
    renderer: null,
    robot: null,
    pcb: null,
  },
  testMode: {
    isEnabled: false,
    motorBind: false,
    servoBind: false,
    foc_mode: 0,
    ser_mode: 0,
    values: {
      motor1: 0,
      motor2: 0,
      servo1: 0,
      servo2: 0,
    },
  },
};

// 2. DOM 元素缓存
function getElement(id) {
  const element = document.getElementById(id);
  if (!element) {
    // 允许在测试环境中某些元素不存在
    console.warn(`Element with ID '${id}' not found.`);
    return null;
  }
  return element;
}
/**
 * DOM 元素引用
 * 统一在此处获取，方便管理和维护
 */
export const domElements = {
  // Toolbar
  btnSetRate: getElement("btnSetRate"),
  rateHzInput: getElement("rateHz"),
  runSwitch: getElement("runSwitch"),
  chartSwitch: getElement("chartSwitch"),
  testModeSwitch: getElement("testModeSwitch"),
  fallDetectSwitch: getElement("fallDetectSwitch"),
  statusLabel: getElement("status"),

  // Indicators
  fallLamp: getElement("fallLamp"),
  fallLabel: getElement("fallLabel"),
  voltageLamp: getElement("voltageLamp"),
  voltageLabel: getElement("voltageLabel"),

  // Charts
  chart1: {
    canvas: getElement("chart1"),
    title: getElement("chartTitle1"),
  },
  chart2: {
    canvas: getElement("chart2"),
    title: getElement("chartTitle2"),
  },
  chart3: {
    canvas: getElement("chart3"),
    title: getElement("chartTitle3"),
  },

  // PID Controls
  pidCard: getElement("pidCard"),
  btnPidSend: getElement("btnPidSend"),
  btnPidPull: getElement("btnPidPull"),

  // Test Mode
  testModeCard: getElement("testModeCard"),
  focModeBtns: getElement("focModeBtns"),
  servoModeBtns: getElement("servoModeBtns"),
  motorBindSwitch: getElement("motorBindSwitch"),
  servoBindSwitch: getElement("servoBindSwitch"),
  motor1TestSlider: getElement("motor1TestSlider"),
  motor2TestSlider: getElement("motor2TestSlider"),
  servo1TestSlider: getElement("servo1TestSlider"),
  servo2TestSlider: getElement("servo2TestSlider"),
  motor1TestValue: getElement("motor1TestValue"),
  motor2TestValue: getElement("motor2TestValue"),
  servo1TestValue: getElement("servo1TestValue"),
  servo2TestValue: getElement("servo2TestValue"),
  // 3D View
  robotCanvas: getElement("robotCanvas"),
  btnZeroAtt: getElement("btnZeroAtt"),
  attOut: getElement("attOut"),
  // Joystick
  joystick: getElement("joystick"),
  stick: getElement("stick"),
  joyOut: getElement("joyOut"),

  // Log
  log: getElement("log"),
};

/**
 * 全局常量
 */
export const CONSTANTS = {
  MAX_CHART_POINTS: 300,
  JOYSTICK_SEND_INTERVAL: 50, // ms, 20Hz
  VOLTAGE_LOW_THRESHOLD: 7.4, // V
};

// PID 参数键名列表
export const PID_KEYS = Array.from({ length: 15 }, (_, i) => {
  const key = `key${String(i + 1).padStart(2, "0")}`;
  // 参数13, 14, 15 的小数位数为 3
  return { k: key, fix: 3 };
});
