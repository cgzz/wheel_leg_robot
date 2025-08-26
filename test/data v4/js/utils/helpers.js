// /js/utils/helpers.js

/**
 * 简化的DOM元素选择器。
 *
 * @param {string} selector - CSS选择器字符串。
 * @returns {HTMLElement | null} 返回匹配的第一个DOM元素，如果没有则返回null。
 */
export const $ = (selector) => document.querySelector(selector);

/**
 * 将数值限制在[-10, 10]的范围内。
 *
 * @param {any} v - 输入值。
 * @returns {number} 限制后的数值。如果输入值不是有限数字，则返回0。
 */
export function clamp10(v) {
    const num = parseFloat(v);
    if (!Number.isFinite(num)) {
        return 0;
    }
    return Math.max(-10, Math.min(10, num));
}