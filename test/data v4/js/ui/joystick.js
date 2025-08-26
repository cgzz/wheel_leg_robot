// /js/ui/joystick.js

import { elements } from "./dom-elements.js";
import { wsSend } from "../core/websocket.js";
import { appendLog } from "./logger.js";

let isDragging = false;
let joystickRect;
let centerX, centerY;
let radius;

/**
 * 更新摇杆的UI显示和WebSocket数据发送，此逻辑完全复刻自原始代码。
 * @param {PointerEvent} e - Pointer事件对象。
 */
function updateJoystick(e) {
  if (!isDragging) return;

  const { stick, joyOut } = elements;

  let x = e.clientX - centerX;
  let y = e.clientY - centerY;

  const mag = Math.sqrt(x * x + y * y);
  if (mag > radius) {
    x = (x / mag) * radius;
    y = (y / mag) * radius;
  }

  stick.style.transform = `translate(${x}px, ${y}px)`;

  const joyX = x / radius;
  const joyY = y / radius;
  const joyR = Math.sqrt(joyX * joyX + joyY * joyY);
  const joyA = (Math.atan2(joyY, joyX) * 180) / Math.PI;

  joyOut.textContent = `x: ${joyX.toFixed(2)} y: ${joyY.toFixed(
    2
  )} r: ${joyR.toFixed(2)} θ: ${joyA.toFixed(1)}°`;
  wsSend({ type: "joy", x: joyX, y: joyY, a: joyA });
}

/**
 * 按下摇杆时的处理函数。
 * @param {PointerEvent} e - Pointer事件对象。
 */
function onPointerDown(e) {
  isDragging = true;
  elements.stick.style.transition = "none"; // 按下时移除过渡效果，响应更灵敏
  joystickRect = elements.joystick.getBoundingClientRect();
  centerX = joystickRect.left + joystickRect.width / 2;
  centerY = joystickRect.top + joystickRect.height / 2;
  radius = joystickRect.width / 2 - elements.stick.offsetWidth / 2;
  updateJoystick(e);
}

/**
 * 松开摇杆时的处理函数。
 */
function onPointerUp() {
  if (!isDragging) return;
  isDragging = false;

  const { stick, joyOut } = elements;
  stick.style.transition = "transform 0.2s ease-out"; // 松开时增加回弹动画
  stick.style.transform = "translate(0px, 0px)";

  joyOut.textContent = "x: 0.00 y: 0.00 r: 0.00 θ: 0.0°";
  wsSend({ type: "joy", x: 0, y: 0, a: 0 });
}

/**
 * 初始化遥控杆模块，绑定所有事件。
 */
export function initJoystick() {
  if (!elements.joystick || !elements.stick) return;

  elements.joystick.addEventListener("pointerdown", onPointerDown);
  window.addEventListener("pointermove", updateJoystick);
  window.addEventListener("pointerup", onPointerUp);

  // 当鼠标移出窗口时也判定为松开
  document.body.addEventListener("pointerleave", onPointerUp);

  appendLog("[INIT] Joystick module initialized.");
}
