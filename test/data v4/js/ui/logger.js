// /js/ui/logger.js

import { elements } from './dom-elements.js';

/**
 * 初始化日志模块。
 * 这个函数目前是空的，为未来可能需要的初始化逻辑预留位置。
 */
export function initLogger() {
    //
}

/**
 * 向事件日志区域追加一条新日志。
 * @param {string} text - 需要追加的日志文本。
 */
export function appendLog(text) {
  if (!elements.log) return;
  
  const time = new Date().toLocaleTimeString();
  const newLog = `\n[${time}] ${text}`;
  
  elements.log.textContent += newLog;
  
  // 自动滚动到日志底部
  elements.log.scrollTop = elements.log.scrollHeight;
}

/**
 * 设置顶部工具栏的全局状态显示文本。
 * @param {string} text - 要显示的状态文本，如 'connected', 'reconnecting…'。
 */
export function setStatus(text) {
  if (elements.statusLabel) {
    elements.statusLabel.textContent = `状态: ${text}`;
  }
}