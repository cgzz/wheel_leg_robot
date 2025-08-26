// /js/app.js
import { state } from "./state.js";
import { $, appendLog, logLine, setStatus } from "./dom.js";
import { initCharts } from "./charts.js";
import { init3D } from "./att3d.js";
import { initPID } from "./pid.js";
import { initJoystick } from "./joystick.js";
import { openWS, wsSend, syncState, setOnTelemetry } from "./ws.js";
import { initTestMode, enableTestMode } from "./testmode.js";

// 摔倒指示灯：null/undefined => off（灰），true => 红，false => 绿
function updateFallIndicator(fallState) {
  const lamp = document.querySelector("#fallLamp");
  const label = document.querySelector("#fallLabel");
  if (!lamp || !label) return;

  lamp.classList.remove("green", "red", "off");

  if (fallState === null || fallState === undefined) {
    lamp.classList.add("off");
    label.textContent = "—";
  } else if (fallState) {
    lamp.classList.add("red");
    label.textContent = "已摔倒";
  } else {
    lamp.classList.add("green");
    label.textContent = "稳定";
  }
}
// 更新电压指示灯
function updateVoltageIndicator(voltage) {
  const lamp = document.querySelector("#voltageLamp");
  const label = document.querySelector("#voltageLabel");
  if (!lamp || !label) return;

  lamp.classList.remove("green", "red", "off");

  // 处理无效或未接收到的电压值
  if (voltage === null || voltage === undefined || !Number.isFinite(voltage)) {
    lamp.classList.add("off");
    label.textContent = "— V";
    return;
  }

  const isLow = voltage < 7.9;
  lamp.classList.add(isLow ? "red" : "green"); // 低于7.9V为红灯，否则为绿灯
  label.textContent = voltage.toFixed(2) + "V";
}
// 按钮按下效果（PC+触控）
function initButtonPressEffects() {
  const add = (el) => {
    el.addEventListener("pointerdown", () => el.classList.add("is-press"));
    const off = () => el.classList.remove("is-press");
    el.addEventListener("pointerup", off);
    el.addEventListener("pointerleave", off);
    el.addEventListener("pointercancel", off);
  };
  document.querySelectorAll(".btn").forEach(add);

  // —— 动态元素兜底（修复 e.target.closest 报错）——
  document.addEventListener("pointerdown", (e) => {
    const trg = e.target;
    if (!(trg instanceof Element)) return; // 只有元素节点才有 closest
    const t = trg.closest?.(".btn");
    if (t) t.classList.add("is-press");
  });

  const off = (e) => {
    const trg = e.target;
    if (!(trg instanceof Element)) return; // 防止 document/window
    const t = trg.closest?.(".btn");
    if (t) t.classList.remove("is-press");
  };
  document.addEventListener("pointerup", off);
  document.addEventListener("pointerleave", off);
  document.addEventListener("pointercancel", off);
}

function bindTopbar() {
  $("#btnSetRate").onclick = () => {
    const hz = Math.max(
      1,
      Math.min(60, parseInt($("#rateHz").value || "10", 10))
    );
    wsSend({ type: "telem_hz", hz });
    appendLog("[SEND] telem_hz " + hz + " Hz");
  };

  const runSwitch = $("#runSwitch");
  if (runSwitch) {
    runSwitch.onchange = () => {
      wsSend({ type: "robot_run", running: runSwitch.checked });
      appendLog("[SEND] robot_run " + (runSwitch.checked ? "on" : "off"));
    };
  }

  const chartSwitch = $("#chartSwitch");
  if (chartSwitch) {
    chartSwitch.onchange = () => {
      wsSend({ type: "charts_send", on: chartSwitch.checked });
      state.chartsOn = chartSwitch.checked;
      appendLog("[INFO] charts " + (state.chartsOn ? "enabled" : "disabled"));
      if (!state.chartsOn) updateFallIndicator(null);
      if (!state.chartsOn) updateVoltageIndicator(null);
    };
  }

  const btnZeroAtt = $("#btnZeroAtt");
  if (btnZeroAtt) {
    btnZeroAtt.onclick = () => {
      wsSend({ type: "imu_restart" });
      appendLog("[INFO] attitude zeroed (roll/yaw)");
    };
  }

  const testModeSwitch = $("#testModeSwitch");
  if (testModeSwitch) {
    testModeSwitch.onchange = () => {
      enableTestMode(testModeSwitch.checked);
    };
  }

  const fallDetectSwitch = $("#fallDetectSwitch");
  if (fallDetectSwitch) {
    fallDetectSwitch.onchange = () => {
      wsSend({ type: "fall_check", enable: fallDetectSwitch.checked });
      appendLog(
        "[SEND] fall_check " + (fallDetectSwitch.checked ? "on" : "off")
      );
    };
  }

  document.addEventListener("visibilitychange", () => {
    state.vis = !document.hidden;
  });
}

async function init() {
  setStatus("connecting…");
  bindTopbar();
  initCharts();
  initPID();
  initJoystick();
  init3D();
  initTestMode();
  initButtonPressEffects();

  // 启动时灯先置灰
  updateFallIndicator(null);
  updateVoltageIndicator(null);

  // 订阅遥测：图表推送开启时才更新摔倒灯
  setOnTelemetry((msg) => {
    if (state.chartsOn && typeof msg.fallen !== "undefined") {
      updateFallIndicator(msg.fallen);
      updateVoltageIndicator(msg.voltage);
    }
  });

  await syncState();
  openWS();
  logLine("ready");
}

window.addEventListener("DOMContentLoaded", init);
