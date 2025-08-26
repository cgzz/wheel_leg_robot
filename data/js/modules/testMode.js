// /assets/js/modules/testMode.js
import { state, domElements } from "../config.js";
import { sendWebSocketMessage } from "../services/websocket.js";

/**
 * 设置模式按钮组的激活状态
 * @param {HTMLElement} btnGroup - The button group element
 * @param {number} mode - The mode to activate (0, 1, or 2)
 */
function setActiveButton(btnGroup, mode) {
  if (!btnGroup) return; // 添加安全检查
  btnGroup.querySelectorAll("button").forEach((b) => {
    b.classList.remove("active");
  });
  const activeButton = btnGroup.querySelector(`button[data-mode='${mode}']`);
  if (activeButton) {
    activeButton.classList.add("active");
  }
}

/**
 * 发送测试状态到后端
 */
function sendTestState() {
  const { isEnabled, foc_mode, ser_mode, values } = state.testMode;
  sendWebSocketMessage({
    type: "test_mode",
    enabled: isEnabled,
    foc_mode: foc_mode,
    ser_mode: ser_mode,
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
    focModeBtns,
    servoModeBtns,
    motorBindSwitch,
    servoBindSwitch,
    motor1TestSlider,
    motor2TestSlider,
    servo1TestSlider,
    servo2TestSlider,
    motor1TestValue,
    motor2TestValue,
    servo1TestValue,
    servo2TestValue,
  } = domElements;

  // 为滑条和数字输入框创建同步更新的函数
  const setupSliderSync = (slider, valueInput, stateKey, isMotor) => {
    if (!slider || !valueInput) {
      console.warn(
        `[testMode] Missing slider or value input for ${stateKey}. Check home.html and config.js.`
      );
      return;
    }

    slider.addEventListener("input", () => {
      const value = parseFloat(slider.value);
      valueInput.value = value;
      state.testMode.values[stateKey] = value;
      sendTestState();
    });

    valueInput.addEventListener("change", () => {
      let value = parseFloat(valueInput.value);
      if (isNaN(value)) value = 0; // 处理无效输入
      // 限制输入值在滑条范围内
      value = Math.max(slider.min, Math.min(slider.max, value));
      valueInput.value = value; // 更新显示值
      slider.value = value;
      state.testMode.values[stateKey] = value;
      sendTestState();
    });

    // 在初始化时，确保两者同步
    slider.value = valueInput.value;
  };

  // 绑定模式按钮事件
  if (focModeBtns) {
    focModeBtns.querySelectorAll("button").forEach((btn) => {
      btn.addEventListener("click", () => {
        state.testMode.foc_mode = parseInt(btn.dataset.mode, 10);
        setActiveButton(focModeBtns, state.testMode.foc_mode);
        sendTestState();
      });
    });
  }

  if (servoModeBtns) {
    servoModeBtns.querySelectorAll("button").forEach((btn) => {
      btn.addEventListener("click", () => {
        state.testMode.ser_mode = parseInt(btn.dataset.mode, 10);
        setActiveButton(servoModeBtns, state.testMode.ser_mode);
        sendTestState();
      });
    });
  }

  // 绑定开关事件
  if (motorBindSwitch) {
    motorBindSwitch.addEventListener("change", () => {
      state.testMode.motorBind = motorBindSwitch.checked;
      if (motorBindSwitch.checked) {
        const value = parseFloat(motor1TestSlider.value);
        if (motor2TestSlider) motor2TestSlider.value = value;
        if (motor2TestValue) motor2TestValue.value = value;
        state.testMode.values.motor2 = value;
        sendTestState();
      }
    });
  }

  if (servoBindSwitch) {
    servoBindSwitch.addEventListener("change", () => {
      state.testMode.servoBind = servoBindSwitch.checked;
      if (servoBindSwitch.checked) {
        const value = parseFloat(servo1TestSlider.value);
        if (servo2TestSlider) servo2TestSlider.value = value;
        if (servo2TestValue) servo2TestValue.value = value;
        state.testMode.values.servo2 = value;
        sendTestState();
      }
    });
  }

  // 设置所有滑条
  setupSliderSync(motor1TestSlider, motor1TestValue, "motor1", true);
  setupSliderSync(motor2TestSlider, motor2TestValue, "motor2", true);
  setupSliderSync(servo1TestSlider, servo1TestValue, "servo1", false);
  setupSliderSync(servo2TestSlider, servo2TestValue, "servo2", false);

  // 初始化UI状态
  setActiveButton(focModeBtns, state.testMode.foc_mode);
  setActiveButton(servoModeBtns, state.testMode.ser_mode);
}

/**
 * 启用或禁用测试模式
 * @param {boolean} enabled
 */
export function enableTestMode(enabled) {
  state.testMode.isEnabled = !!enabled;
  const { pidCard, testModeCard } = domElements;
  if (!pidCard || !testModeCard) return;

  if (state.testMode.isEnabled) {
    pidCard.classList.add("hidden");
    testModeCard.classList.remove("hidden");
    sendTestState();
  } else {
    testModeCard.classList.add("hidden");
    pidCard.classList.remove("hidden");
  }

  sendWebSocketMessage({
    type: "test_mode",
    enabled: state.testMode.isEnabled,
  });
}
