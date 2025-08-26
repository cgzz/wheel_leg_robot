// /js/ui/pid.js

import { elements } from './dom-elements.js';
import { wsSend } from '../core/websocket.js';
import { appendLog } from './logger.js';

/**
 * 将从UI收集的所有滑块值打包成一个对象。
 * @returns {object} 包含所有PID键值对的对象。
 */
function packagePidData() {
    const param = {};
    for (let i = 1; i <= 13; i++) {
        const key = `key${i.toString().padStart(2, '0')}`;
        const rangeElement = elements[`pidRg_${key}`];
        if (rangeElement) {
            param[key] = parseFloat(rangeElement.value);
        }
    }
    return param;
}

/**
 * "发送" 按钮的事件处理函数。
 */
function handlePidSend() {
    const pidData = packagePidData();
    wsSend({ type: 'set_pid', param: pidData });
    appendLog('[SEND] set_pid');
}

/**
 * "更新" 按钮的事件处理函数。
 */
function handlePidPull() {
    wsSend({ type: 'get_pid' });
    appendLog('[SEND] get_pid');
}

/**
 * 初始化PID控制模块，为所有相关的滑块、输入框和按钮绑定事件。
 */
export function initPidControls() {
    elements.btnPidSend.addEventListener('click', handlePidSend);
    elements.btnPidPull.addEventListener('click', handlePidPull);

    // 为所有13个PID参数设置联动事件
    for (let i = 1; i <= 13; i++) {
        const key = `key${i.toString().padStart(2, '0')}`;
        const rangeInput = elements[`pidRg_${key}`];
        const numberInput = elements[`pidNv_${key}`];
        const staticValue = elements[`pidSv_${key}`];

        if (!rangeInput || !numberInput || !staticValue) continue;

        // 滑块 -> 数字输入框 和 显示值
        rangeInput.addEventListener('input', () => {
            const value = parseFloat(rangeInput.value);
            const step = parseFloat(rangeInput.step) || 0.01;
            const decimals = step.toString().split('.')[1]?.length || 2;
            
            numberInput.value = value.toFixed(decimals);
            staticValue.textContent = value.toFixed(decimals);
        });

        // 数字输入框 -> 滑块
        numberInput.addEventListener('change', () => {
            const value = parseFloat(numberInput.value);
            rangeInput.value = value;
            // 触发input事件以更新静态显示值
            rangeInput.dispatchEvent(new Event('input')); 
        });
    }
}

/**
 * 根据从后端接收的数据，填充PID控制面板的UI。
 * @param {object} param - 包含PID键值对的对象。
 */
export function fillPidToUI(param) {
    appendLog('[RECV] pid data');
    for (const key in param) {
        if (Object.hasOwnProperty.call(param, key)) {
            const rangeInput = elements[`pidRg_${key}`];
            if (rangeInput) {
                rangeInput.value = param[key];
                // 手动触发input事件，以确保数字输入框和静态文本也同步更新
                rangeInput.dispatchEvent(new Event('input'));
            }
        }
    }
}

/**
 * 根据从后端接收的UI配置，更新PID滑块的标签和分组名称。
 * @param {Array} sliderConfigs - 滑块配置数组。
 */
export function applySliderConfig(sliderConfigs) {
    sliderConfigs.forEach((groupConfig, groupIndex) => {
        const groupElement = elements[`pidGroup${groupIndex + 1}`];
        if (groupElement) {
            groupElement.textContent = groupConfig.group;
        }

        groupConfig.names.forEach((name, nameIndex) => {
            const labelElement = elements[`pid${groupIndex + 1}Label${nameIndex + 1}`];
            if (labelElement) {
                labelElement.textContent = name;
            }
        });
    });
    appendLog('[INIT] UI Sliders configured.');
}