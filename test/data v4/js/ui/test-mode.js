// /js/ui/test-mode.js

import { elements } from './dom-elements.js';
import { wsSend } from '../core/websocket.js';
import { clamp10 } from '../utils/helpers.js';

// 模块内部状态
let currentMode = 0; // 0: 速度, 1: 力矩, 2: 位置
let isEnabled = false;

/**
 * 设置当前激活的模式按钮的高亮状态。
 * @param {string | number} mode - 当前的模式值 ('0', '1', '2')。
 */
function setActiveButton(mode) {
    const btns = Array.from(elements.modeBtns.querySelectorAll('button[data-mode]'));
    btns.forEach(b => b.classList.toggle('active', b.dataset.mode === mode.toString()));
}

/**
 * 向后端发送测试模式的当前状态。
 * @param {number | string} rawValue - 从UI控件获取的原始值。
 * @param {boolean} [once=false] - 是否为一次性发送（通常用于模式切换）。
 */
function sendState(rawValue, once = false) {
    const value = clamp10(rawValue);
    wsSend({
        type: 'test_mode',
        enabled: isEnabled,
        mode: parseInt(currentMode, 10), // 修正：使用正确的 currentMode 变量
        value,
        once
    });
}

/**
 * 初始化测试模式模块，绑定所有事件监听器。
 */
export function initTestMode() {
    const { testValueSlider, testValueNum, testValOut, modeBtns } = elements;

    // 更新数值显示的辅助函数
    const syncOut = (v) => {
        testValOut.textContent = clamp10(v).toFixed(2);
    };

    // 模式切换按钮事件
    const modeButtons = Array.from(modeBtns.querySelectorAll('button[data-mode]'));
    setActiveButton(currentMode);
    modeButtons.forEach(btn => {
        btn.addEventListener('click', () => {
            currentMode = btn.dataset.mode;
            setActiveButton(currentMode);
            // 模式切换时，发送一次当前值，便于后端切换通道
            sendState(testValueSlider.value, true);
        });
    });

    // 滑块与数字输入框的联动逻辑
    const onSliderInput = () => {
        const v = clamp10(testValueSlider.value);
        testValueNum.value = v.toFixed(2);
        syncOut(v);
        if (isEnabled) {
            sendState(v);
        }
    };
    testValueSlider.addEventListener('input', onSliderInput);

    const onNumberChange = () => {
        const v = clamp10(testValueNum.value);
        // 同步回输入框，避免用户输入超出范围的值
        testValueNum.value = v.toFixed(2);
        testValueSlider.value = v;
        syncOut(v);
        if (isEnabled) {
            sendState(v);
        }
    };
    testValueNum.addEventListener('change', onNumberChange);

    // 初始化一次数值显示
    syncOut(testValueSlider.value);
}

/**
 * 启用或禁用测试模式。
 * @param {boolean} on - true为启用，false为禁用。
 */
export function enableTestMode(on) {
    isEnabled = !!on;
    const { pidCard, testModeCard } = elements;

    // 切换PID面板和测试模式面板的可见性
    pidCard.classList.toggle('hidden', isEnabled);
    testModeCard.classList.toggle('hidden', !isEnabled);

    // 通知后端进入或退出测试模式
    wsSend({ 
        type: 'test_mode', 
        enabled: isEnabled, 
        mode: parseInt(currentMode, 10) // 修正：使用正确的 currentMode 变量
    });
}