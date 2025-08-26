// /assets/js/modules/testMode.js
import { state, domElements } from '../config.js';
import { sendWebSocketMessage } from '../services/websocket.js';

function setActiveButton(mode) {
  domElements.modeBtns.querySelectorAll('button').forEach(b => {
    b.classList.toggle('active', b.dataset.mode === String(mode));
  });
}

function clampValue(v) {
  if (!Number.isFinite(v)) return 0;
  return Math.max(-10, Math.min(10, v));
}

function sendTestState(value, once = false) {
  const { isEnabled, currentMode } = state.testMode;
  sendWebSocketMessage({
    type: 'test_mode',
    enabled: isEnabled,
    currentMode,
    value: clampValue(value),
    once
  });
}

export function initTestMode() {
  const { testValueSlider, testValueNumInput, testValOut, modeBtns } = domElements;

  // Sync output display
  const syncOutput = (v) => {
    testValOut.textContent = clampValue(v).toFixed(2);
  };

  // Mode buttons
  modeBtns.querySelectorAll('button').forEach(btn => {
    btn.addEventListener('click', () => {
      state.testMode.currentMode = parseInt(btn.dataset.mode, 10);
      setActiveButton(state.testMode.currentMode);
      const currentValue = parseFloat(testValueSlider.value || '0');
      sendTestState(currentValue, true); // Send once on mode change
    });
  });

  // Slider -> Number input
  const onSliderChange = () => {
    const value = parseFloat(testValueSlider.value || '0');
    testValueNumInput.value = value.toFixed(2);
    syncOutput(value);
    if (state.testMode.isEnabled) {
      sendTestState(value, false);
    }
  };
  testValueSlider.addEventListener('input', onSliderChange);
  testValueSlider.addEventListener('change', onSliderChange); // For final value

  // Number input -> Slider
  const onNumberChange = () => {
    let value = parseFloat(testValueNumInput.value);
    if (!Number.isFinite(value)) value = 0;
    
    value = clampValue(value);
    // Sync back to input box to prevent out-of-range values
    testValueNumInput.value = value.toFixed(2);
    testValueSlider.value = value;
    syncOutput(value);
    if (state.testMode.isEnabled) {
      sendTestState(value, false);
    }
  };
  testValueNumInput.addEventListener('input', onNumberChange);
  testValueNumInput.addEventListener('change', onNumberChange);
  testValueNumInput.addEventListener('keydown', (e) => {
    if (e.key === 'Enter') onNumberChange();
  });

  // Initial UI sync
  setActiveButton(state.testMode.currentMode);
  onSliderChange();
}

/**
 * Enables or disables the test mode UI.
 * @param {boolean} enabled 
 */
export function enableTestMode(enabled) {
  state.testMode.isEnabled = !!enabled;
  const { pidCard, testModeCard } = domElements;
  
  if (state.testMode.isEnabled) {
    pidCard.classList.add('hidden');
    testModeCard.classList.remove('hidden');
  } else {
    testModeCard.classList.add('hidden');
    pidCard.classList.remove('hidden');
  }
  
  // Notify backend of the mode switch (enter/exit)
  sendWebSocketMessage({ type: 'test_mode', enabled: state.testMode.isEnabled, currentMode: state.testMode.currentMode });
}