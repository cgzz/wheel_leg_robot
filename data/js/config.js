// /assets/js/config.js

/**
 * 全局状态管理
 */
export const state = {
  connected: false,
  chartsOn: false,
  isPageVisible: true,
  joystick: { x: 0, y: 0, deg: 0, isDragging: false, lastSendTime: 0 },
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
    currentMode: 0, // 0: speed, 1: torque, 2: position
    motorBind: false, // 新增
    servoBind: false, // 新增
    values: {
      // 新增
      motor1: 0,
      motor2: 0,
      servo1: 0,
      servo2: 0,
    },
  },
};

/**
 * DOM 元素引用
 * 统一在此处获取，方便管理和维护
 */
export const domElements = {
  // Toolbar
  btnSetRate: document.getElementById("btnSetRate"),
  rateHzInput: document.getElementById("rateHz"),
  runSwitch: document.getElementById("runSwitch"),
  chartSwitch: document.getElementById("chartSwitch"),
  testModeSwitch: document.getElementById("testModeSwitch"),
  fallDetectSwitch: document.getElementById("fallDetectSwitch"),
  statusLabel: document.getElementById("status"),

  // Indicators
  fallLamp: document.getElementById("fallLamp"),
  fallLabel: document.getElementById("fallLabel"),
  voltageLamp: document.getElementById("voltageLamp"),
  voltageLabel: document.getElementById("voltageLabel"),

  // Charts
  chart1: {
    canvas: document.getElementById("chart1"),
    title: document.getElementById("chartTitle1"),
  },
  chart2: {
    canvas: document.getElementById("chart2"),
    title: document.getElementById("chartTitle2"),
  },
  chart3: {
    canvas: document.getElementById("chart3"),
    title: document.getElementById("chartTitle3"),
  },

  // PID Controls
  pidCard: document.getElementById("pidCard"),
  btnPidSend: document.getElementById("btnPidSend"),
  btnPidPull: document.getElementById("btnPidPull"),

  // Test Mode
  testModeCard: document.getElementById("testModeCard"),
  modeBtns: document.getElementById("modeBtns"),
  motorBindSwitch: document.getElementById("motorBindSwitch"),
  servoBindSwitch: document.getElementById("servoBindSwitch"),
  motor1TestSlider: document.getElementById("motor1TestValue"),
  motor2TestSlider: document.getElementById("motor2TestValue"),
  servo1TestSlider: document.getElementById("servo1TestValue"),
  servo2TestSlider: document.getElementById("servo2TestValue"),

  // 3D View
  robotCanvas: document.getElementById("robotCanvas"),
  btnZeroAtt: document.getElementById("btnZeroAtt"),
  attOut: document.getElementById("attOut"),
  heightSlider: document.getElementById("heightSlider"),
  heightVal: document.getElementById("heightVal"),

  // Joystick
  joystick: document.getElementById("joystick"),
  stick: document.getElementById("stick"),
  joyOut: document.getElementById("joyOut"),

  // Log
  log: document.getElementById("log"),
};

/**
 * 全局常量
 */
export const CONSTANTS = {
  MAX_CHART_POINTS: 300,
  JOYSTICK_SEND_INTERVAL: 50, // ms, 20Hz
  VOLTAGE_LOW_THRESHOLD: 7.9, // V
};

// PID 参数键名列表
export const PID_KEYS = Array.from({ length: 15 }, (_, i) => {
  const key = `key${String(i + 1).padStart(2, "0")}`;
  // 参数13, 14, 15 的小数位数为 3
  return { k: key, fix: 3 };
});
