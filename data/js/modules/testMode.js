// /assets/js/modules/testMode.js
import { state, domElements } from "../config.js";
import { sendWebSocketMessage } from "../services/websocket.js";

/**
 * 设置模式按钮的激活状态
 * @param {number} mode - The mode to activate (0, 1, or 2)
 */
function setActiveButton(mode) {
  domElements.modeBtns.querySelectorAll("button").forEach((b) => {
    b.classList.toggle("active", b.dataset.mode === String(mode));
  });
}

/**
 * 发送测试状态到后端
 */
function sendTestState() {
  const { isEnabled, currentMode, values } = state.testMode;
  sendWebSocketMessage({
    type: "test_mode",
    enabled: isEnabled,
    mode: currentMode,
    motor1: values.motor1,
    motor2: values.motor2,
    servo1: values.servo1,
    servo2: values.servo2,
  });
}

/**
 * 初始化测试模式模块
 */
export function initTestMode() {
  const {
    modeBtns,
    motorBindSwitch,
    servoBindSwitch,
    motor1TestSlider,
    motor2TestSlider,
    servo1TestSlider,
    servo2TestSlider,
  } = domElements;

  // 模式按钮事件
  modeBtns.querySelectorAll("button").forEach((btn) => {
    btn.addEventListener("click", () => {
      state.testMode.currentMode = parseInt(btn.dataset.mode, 10);
      setActiveButton(state.testMode.currentMode);
      sendTestState(); // 模式切换时发送一次当前所有值
    });
  });

  // 绑定开关事件
  motorBindSwitch.addEventListener("change", () => {
    state.testMode.motorBind = motorBindSwitch.checked;
    if (motorBindSwitch.checked) {
      motor2TestSlider.value = motor1TestSlider.value;
      state.testMode.values.motor2 = state.testMode.values.motor1;
      sendTestState();
    }
  });

  servoBindSwitch.addEventListener("change", () => {
    state.testMode.servoBind = servoBindSwitch.checked;
    if (servoBindSwitch.checked) {
      servo2TestSlider.value = servo1TestSlider.value;
      state.testMode.values.servo2 = state.testMode.values.servo1;
      sendTestState();
    }
  });

  // 滑块事件
  motor1TestSlider.addEventListener("input", () => {
    const value = parseFloat(motor1TestSlider.value);
    state.testMode.values.motor1 = value;
    if (state.testMode.motorBind) {
      motor2TestSlider.value = value;
      state.testMode.values.motor2 = value;
    }
    sendTestState();
  });

  motor2TestSlider.addEventListener("input", () => {
    const value = parseFloat(motor2TestSlider.value);
    state.testMode.values.motor2 = value;
    if (state.testMode.motorBind) {
      motor1TestSlider.value = value;
      state.testMode.values.motor1 = value;
    }
    sendTestState();
  });

  servo1TestSlider.addEventListener("input", () => {
    const value = parseFloat(servo1TestSlider.value);
    state.testMode.values.servo1 = value;
    if (state.testMode.servoBind) {
      servo2TestSlider.value = value;
      state.testMode.values.servo2 = value;
    }
    sendTestState();
  });

  servo2TestSlider.addEventListener("input", () => {
    const value = parseFloat(servo2TestSlider.value);
    state.testMode.values.servo2 = value;
    if (state.testMode.servoBind) {
      servo1TestSlider.value = value;
      state.testMode.values.servo1 = value;
    }
    sendTestState();
  });

  // 初始化UI状态
  setActiveButton(state.testMode.currentMode);
}

/**
 * 启用或禁用测试模式
 * @param {boolean} enabled
 */
export function enableTestMode(enabled) {
  state.testMode.isEnabled = !!enabled;
  const { pidCard, testModeCard } = domElements;

  if (state.testMode.isEnabled) {
    pidCard.classList.add("hidden");
    testModeCard.classList.remove("hidden");
  } else {
    testModeCard.classList.add("hidden");
    pidCard.classList.remove("hidden");
  }

  // 通知后端进入/退出测试模式
  sendWebSocketMessage({
    type: "test_mode",
    enabled: state.testMode.isEnabled,
  });
}
