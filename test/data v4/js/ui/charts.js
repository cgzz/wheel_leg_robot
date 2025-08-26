// /js/ui/charts.js

import { elements } from './dom-elements.js';
import { appendLog } from './logger.js';

let charts = [];
const CHART_BUFFER_SIZE = 50;

/**
 * 绘制图表
 * @param {HTMLCanvasElement} ctx - Canvas上下文
 * @returns {Chart} Chart.js实例
 */
function createChart(ctx) {
    return new Chart(ctx, {
        type: 'line',
        data: {
            labels: [],
            datasets: [
                { label: '通道1', data: [], borderColor: 'rgb(255, 99, 132)', tension: 0.1, pointRadius: 0 },
                { label: '通道2', data: [], borderColor: 'rgb(54, 162, 235)', tension: 0.1, pointRadius: 0 },
                { label: '通道3', data: [], borderColor: 'rgb(75, 192, 192)', tension: 0.1, pointRadius: 0 },
            ],
        },
        options: {
            animation: false,
            scales: {
                x: { display: false },
            },
        },
    });
}

/**
 * 初始化所有图表。
 */
export function initCharts() {
    charts.push(createChart(elements.chart1));
    charts.push(createChart(elements.chart2));
    charts.push(createChart(elements.chart3));
    appendLog('[INIT] Charts initialized.');
}

/**
 * 根据后端配置更新图表的标题和图例。
 * @param {Array<object>} configs - 包含图表标题和图例的配置数组。
 */
export function applyChartConfig(configs) {
    configs.forEach((config, i) => {
        if (!charts[i]) return;

        // 更新图表标题
        const titleElement = elements[`chartTitle${i + 1}`];
        if (titleElement) {
            titleElement.textContent = config.title;
        }

        // 更新图表图例
        config.legends.forEach((legend, j) => {
            if (charts[i].data.datasets[j]) {
                charts[i].data.datasets[j].label = legend;
            }
        });
        charts[i].update('none'); // 'none' 表示不带动画更新
    });
    appendLog('[INIT] UI Charts configured.');
}

/**
 * 向图表添加新的数据点。
 * @param {...number} dataPoints - 接收到的新数据点。
 */
export function updateCharts(...dataPoints) {
    if (!charts.length) return;

    const time = new Date().toLocaleTimeString();

    // 更新每个图表
    charts.forEach((chart, i) => {
        for (let j = 0; j < chart.data.datasets.length && j < 3; j++) {
            const dataIndex = i * 3 + j;
            if (dataIndex < dataPoints.length) {
                const dataset = chart.data.datasets[j];
                dataset.data.push({ x: time, y: dataPoints[dataIndex] });
            }
        }
    });

    // 保持图表数据点的数量
    charts.forEach(chart => {
        chart.data.labels.push(time);
        if (chart.data.labels.length > CHART_BUFFER_SIZE) {
            chart.data.labels.shift();
            chart.data.datasets.forEach(dataset => {
                if (dataset.data.length > 0) {
                    dataset.data.shift();
                }
            });
        }
        chart.update('none');
    });
}