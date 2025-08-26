// /js/ui/dom-elements.js
import { $ } from '../utils/helpers.js';

/**
 * 集中管理所有需要交互的DOM元素引用。
 * 在应用启动时一次性查询，之后所有模块都从这里导入和使用元素，
 * 避免了代码中散落的 `querySelector` 调用，便于维护。
 */
export const elements = {};

/**
 * 初始化函数，查询并填充 elements 对象。
 * 这个函数应该在所有其他UI模块初始化之前被调用。
 */
export function initDomElements() {
    // Toolbar elements
    Object.assign(elements, {
        rateHzInput: $('#rateHz'),
        setRateBtn: $('#btnSetRate'),
        runSwitch: $('#runSwitch'),
        chartSwitch: $('#chartSwitch'),
        testModeSwitch: $('#testModeSwitch'),
        fallDetectSwitch: $('#fallDetectSwitch'),
        statusLabel: $('#status'),
    });

    // Indicator elements
    Object.assign(elements, {
        fallLamp: $('#fallLamp'),
        fallLabel: $('#fallLabel'),
        voltageLamp: $('#voltageLamp'),
        voltageLabel: $('#voltageLabel'),
    });

    // Chart elements
    Object.assign(elements, {
        chart1: $('#chart1'),
        chart2: $('#chart2'),
        chart3: $('#chart3'),
        chartTitle1: $('#chartTitle1'),
        chartTitle2: $('#chartTitle2'),
        chartTitle3: $('#chartTitle3'),
    });

    // PID control elements
    Object.assign(elements, {
        pidCard: $('#pidCard'),
        btnPidSend: $('#btnPidSend'),
        btnPidPull: $('#btnPidPull'),
    });
    // Dynamically get all PID sliders and number boxes
    for (let i = 1; i <= 13; i++) {
        const key = `key${i.toString().padStart(2, '0')}`;
        elements[`pidRg_${key}`] = $(`#rg_${key}`);
        elements[`pidSv_${key}`] = $(`#sv_${key}`);
        elements[`pidNv_${key}`] = $(`#nv_${key}`);
    }
     // Also get PID group and label elements for dynamic updates
    for (let i = 1; i <= 4; i++) {
        elements[`pidGroup${i}`] = $(`#pidGroup${i}`);
        for (let j = 1; j <= 3; j++) {
            elements[`pid${i}Label${j}`] = $(`#pid${i}Label${j}`);
        }
    }


    // Test Mode elements
    Object.assign(elements, {
        testModeCard: $('#testModeCard'),
        modeBtns: $('#modeBtns'),
        testValueSlider: $('#testValue'),
        testValueNum: $('#testValueNum'),
        testValOut: $('#testValOut'),
    });

    // 3D Attitude and Joystick elements
    Object.assign(elements, {
        robotCanvas: $('#robotCanvas'),
        attOut: $('#attOut'),
        btnZeroAtt: $('#btnZeroAtt'),
        heightSlider: $('#heightSlider'),
        heightVal: $('#heightVal'),
        joystick: $('#joystick'),
        stick: $('#stick'),
        joyOut: $('#joyOut'),
    });

    // Logger element
    Object.assign(elements, {
        log: $('#log'),
    });
}