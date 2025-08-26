// /assets/js/modules/joystick.js
import { state, domElements, CONSTANTS } from '../config.js';
import { sendWebSocketMessage } from '../services/websocket.js';

let joystickRadius, stickRadius, maxDisplacement;

function setStickPosition(px, py) {
  domElements.stick.style.left = `${px + joystickRadius}px`;
  domElements.stick.style.top = `${py + joystickRadius}px`;
}

function updateJoystickReadout(px, py) {
  const nx = px / maxDisplacement;
  const ny = -py / maxDisplacement; // Y-axis is inverted for intuitive control
  const r = Math.min(1, Math.hypot(nx, ny));
  const ang = Math.atan2(ny, nx) * 180 / Math.PI;

  domElements.joyOut.textContent = `x: ${nx.toFixed(2)}  y: ${ny.toFixed(2)}  r: ${r.toFixed(2)}  θ: ${ang.toFixed(1)}°`;
  
  state.joystick.x = nx;
  state.joystick.y = ny;
  state.joystick.deg = ang;
}

function getPositionFromEvent(e) {
  const rect = domElements.joystick.getBoundingClientRect();
  const centerX = rect.left + rect.width / 2;
  const centerY = rect.top + rect.height / 2;
  
  const clientX = e.touches ? e.touches[0].clientX : e.clientX;
  const clientY = e.touches ? e.touches[0].clientY : e.clientY;

  const x = clientX - centerX;
  const y = clientY - centerY;
  
  const distance = Math.hypot(x, y);
  if (distance <= maxDisplacement) {
    return { x, y };
  }
  
  // Clamp to the edge of the joystick area
  const k = maxDisplacement / distance;
  return { x: x * k, y: y * k };
}

function sendJoystickData() {
    sendWebSocketMessage({ type: 'joy', x: state.joystick.x, y: state.joystick.y, deg: state.joystick.deg });
}

function handlePointerStart(e) {
  state.joystick.isDragging = true;
  const pos = getPositionFromEvent(e);
  setStickPosition(pos.x, pos.y);
  updateJoystickReadout(pos.x, pos.y);
  sendJoystickData();
}

function handlePointerMove(e) {
  if (!state.joystick.isDragging) return;
  e.preventDefault(); // Prevent page scrolling on touch devices
  const pos = getPositionFromEvent(e);
  setStickPosition(pos.x, pos.y);
  updateJoystickReadout(pos.x, pos.y);
}

function handlePointerEnd() {
  if (!state.joystick.isDragging) return;
  state.joystick.isDragging = false;
  setStickPosition(0, 0);
  updateJoystickReadout(0, 0);
  sendWebSocketMessage({ type: 'joy', x: 0, y: 0, deg: 0 });
}

export function initJoystick() {
  const joy = domElements.joystick;
  joystickRadius = joy.clientWidth / 2;
  stickRadius = domElements.stick.clientWidth / 2;
  maxDisplacement = joystickRadius - stickRadius;
  
  setStickPosition(0, 0);

  // Use pointer events for unified mouse and touch input
  joy.addEventListener('pointerdown', e => { joy.setPointerCapture(e.pointerId); handlePointerStart(e); });
  joy.addEventListener('pointermove', handlePointerMove);
  joy.addEventListener('pointerup', handlePointerEnd);
  joy.addEventListener('pointercancel', handlePointerEnd);
  joy.addEventListener('pointerleave', handlePointerEnd);
  
  // Throttled sending for performance
  setInterval(() => {
    if (!state.joystick.isDragging) return;
    const now = performance.now();
    if (now - state.joystick.lastSendTime < CONSTANTS.JOYSTICK_SEND_INTERVAL) return;
    state.joystick.lastSendTime = now;
    sendJoystickData();
  }, 20); // Check every 20ms, actual send rate is controlled by JOYSTICK_SEND_INTERVAL
}