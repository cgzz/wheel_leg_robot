// /js/ui/attitude3d.js

import { elements } from './dom-elements.js';
import { appendLog } from './logger.js';

// three.js 相关的变量
let scene, camera, renderer, robot;
let yaw = 0, pitch = 0, roll = 0;

/**
 * 创建与原始代码完全一致的3D机器人模型。
 * @returns {THREE.Group} 包含机器人身体和轮子的组合对象。
 */
function createRobotModel() {
    const group = new THREE.Group();

    // Body - 恢复原始尺寸
    const bodyGeometry = new THREE.BoxGeometry(0.2, 0.08, 0.1);
    const bodyMaterial = new THREE.MeshNormalMaterial();
    const body = new THREE.Mesh(bodyGeometry, bodyMaterial);
    group.add(body);

    // Wheels - 恢复原始尺寸、颜色和位置
    const wheelGeometry = new THREE.CylinderGeometry(0.04, 0.04, 0.12, 32);
    const wheelMaterial = new THREE.MeshBasicMaterial({ color: 0x555555 });

    const wheel1 = new THREE.Mesh(wheelGeometry, wheelMaterial);
    wheel1.position.set(0, -0.05, 0); // 原始位置
    // 原始代码中轮子是沿Y轴旋转的，这里保持一致
    group.add(wheel1);

    const wheel2 = new THREE.Mesh(wheelGeometry, wheelMaterial);
    wheel2.position.set(0, 0.05, 0); // 原始位置
    group.add(wheel2);

    return group;
}

/**
 * 动画循环函数。
 */
function animate() {
    requestAnimationFrame(animate);
    
    // 恢复原始的直接旋转逻辑，无平滑处理
    if (robot) {
        robot.rotation.y = yaw;
        robot.rotation.x = pitch;
        robot.rotation.z = roll;
    }
    
    renderer.render(scene, camera);
}

/**
 * 初始化3D姿态显示模块。
 */
export function initAttitude3D() {
    const container = elements.robotCanvas;
    if (!container || typeof THREE === 'undefined') {
        console.error('3D canvas container not found or THREE.js is not loaded.');
        return;
    }

    // Scene
    scene = new THREE.Scene();
    scene.background = new THREE.Color(0xf0f0f0);

    // Camera - 恢复原始位置
    const aspect = container.clientWidth / container.clientHeight;
    camera = new THREE.PerspectiveCamera(75, aspect, 0.1, 100);
    camera.position.set(0.25, 0.25, 0.25);
    camera.lookAt(0, 0, 0);

    // Renderer
    renderer = new THREE.WebGLRenderer({ antialias: true });
    renderer.setSize(container.clientWidth, container.clientHeight);
    container.appendChild(renderer.domElement);
    
    // Robot model
    robot = createRobotModel();
    scene.add(robot);
    
    // Lights
    const ambientLight = new THREE.AmbientLight(0xffffff, 1);
    scene.add(ambientLight);

    // Event Listeners
    elements.btnZeroAtt.addEventListener('click', () => {
        // 重置时直接更新姿态
        updateAttitude(0, 0, 0);
    });

    elements.heightSlider.addEventListener('input', () => {
        const height = parseFloat(elements.heightSlider.value);
        if (robot) {
            robot.position.y = height;
        }
        elements.heightVal.textContent = `${height.toFixed(2)} m`;
    });

    // Handle window resize
    window.addEventListener('resize', () => {
        if (!container) return;
        const width = container.clientWidth;
        const height = container.clientHeight;
        camera.aspect = width / height;
        camera.updateProjectionMatrix();
        renderer.setSize(width, height);
    }, false);

    animate();
    appendLog('[INIT] 3D Attitude module initialized.');
}

/**
 * 更新姿态数据，供动画循环使用。
 * @param {number} p - Pitch value from backend.
 * @param {number} r - Roll value from backend.
 * @param {number} y - Yaw value from backend.
 */
export function updateAttitude(p, r, y) {
    // 将后端数据（角度）转换为 three.js 使用的弧度
    pitch = p * Math.PI / 180;
    roll = r * Math.PI / 180;
    yaw = y * Math.PI / 180;

    // 更新读数显示
    if (elements.attOut) {
        elements.attOut.textContent = `pitch: ${p.toFixed(1)}° roll: ${r.toFixed(1)}° yaw: ${y.toFixed(1)}°`;
    }
}