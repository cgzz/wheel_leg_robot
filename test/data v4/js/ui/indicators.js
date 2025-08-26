// /js/ui/indicators.js

import { elements } from './dom-elements.js';

/**
 * 初始化指示灯模块。
 * 目前无需执行任何操作，为未来扩展预留。
 */
export function initIndicators() {
    //
}

/**
 * 更新摔倒指示灯的函数。
 * @param {boolean | null | undefined} fallState - 摔倒状态 (true/false/null/undefined)。
 */
function updateFallIndicator(fallState) {
    const { fallLamp, fallLabel } = elements;
    if (!fallLamp || !fallLabel) return;

    // 先重置样式
    fallLamp.className = 'bulb'; 

    if (fallState === null || typeof fallState === 'undefined') {
        fallLamp.classList.add('off'); // 灰色
        fallLabel.textContent = '—';
    } else if (fallState) {
        fallLamp.classList.add('red'); // 红色
        fallLabel.textContent = '已摔倒';
    } else {
        fallLamp.classList.add('green'); // 绿色
        fallLabel.textContent = '稳定';
    }
}

/**
 * 更新电压指示灯的函数。
 * @param {number | null | undefined} voltage - 电压值。
 */
function updateVoltageIndicator(voltage) {
    const { voltageLamp, voltageLabel } = elements;
    if (!voltageLamp || !voltageLabel) return;
    
    // 先重置样式
    voltageLamp.className = 'bulb';

    if (voltage === null || !Number.isFinite(voltage)) {
        voltageLamp.classList.add('off'); // 灰色
        voltageLabel.textContent = '— V';
    } else {
        const isLow = voltage < 7.9;
        voltageLamp.classList.add(isLow ? 'red' : 'green'); // 低于7.9V为红色，否则为绿色
        voltageLabel.textContent = `${voltage.toFixed(2)}V`;
    }
}

/**
 * 统一更新所有指示灯。此函数由 websocket.js 模块调用。
 * @param {boolean | null | undefined} fallState - 摔倒状态。
 * @param {number | null | undefined} voltage - 电压值。
 */
export function updateIndicators(fallState, voltage) {
    updateFallIndicator(fallState);
    updateVoltageIndicator(voltage);
}