// /js/att3d.js
import { state } from './state.js';
import { $, appendLog } from './dom.js';

const mkMat = (hex,m=.1,r=.6)=> new THREE.MeshStandardMaterial({ color:hex, metalness:m, roughness:r });
const mkBox = (w,h,d,mat)=> new THREE.Mesh(new THREE.BoxGeometry(w,h,d), mat);
const mkCyl = (rT,rB,h,mat,seg=36)=> new THREE.Mesh(new THREE.CylinderGeometry(rT,rB,h,seg), mat);

function createMainWheel(radius=0.18, width=0.08){
  const grp = new THREE.Group();
  const tire = mkCyl(radius, radius, width, mkMat(0x2a2d31, .25, .4), 36); tire.rotation.z = Math.PI/2; grp.add(tire);
  const rim  = mkCyl(radius*.82, radius*.82, width*.55, mkMat(0x606a76, .25, .55), 36); rim.rotation.z = Math.PI/2; grp.add(rim);
  const hub  = mkCyl(radius*.28, radius*.28, width*.7, mkMat(0x9aa3ad, .2, .4), 24); hub.rotation.z = Math.PI/2; grp.add(hub);
  return grp;
}
function createChassis(){
  const body = new THREE.Group();
  const pcb = mkBox(0.90,0.02,0.44,mkMat(0x1f7a42,.15,.5));
  const base = mkBox(1.05,0.10,0.52,mkMat(0x3a3f46,.3,.5)); base.position.y=0.35; body.add(base);
  const mid  = mkBox(1.00,0.40,0.50,mkMat(0x5a6069,.2,.6));  mid.position.y=0.60;  body.add(mid);
  const lid  = mkBox(0.98,0.06,0.48,mkMat(0xd7dbe2,.1,.9));  lid.position.y=0.83;  body.add(lid);
  pcb.position.y=0.87; body.add(pcb);
  // 略：倒角/连杆…（保持你原来形状即可）
  return { body, pcb };
}

export function init3D(){
  if (typeof THREE === 'undefined') { appendLog('[WARN] Three.js 未加载'); return; }
  const host = $('#robotCanvas'); host.innerHTML = "";
  const scene = new THREE.Scene(); scene.background = new THREE.Color(0xf6f7fb);
  const w = host.clientWidth, h = 360;
  const camera = new THREE.PerspectiveCamera(40, w/h, .1, 100);
  const renderer = new THREE.WebGLRenderer({ antialias: true });
  renderer.setSize(w,h); renderer.setPixelRatio(window.devicePixelRatio||1);
  host.appendChild(renderer.domElement);
  scene.add(new THREE.HemisphereLight(0xffffff,0x8899aa,.95));
  const dir = new THREE.DirectionalLight(0xffffff,.8); dir.position.set(3,5,2); scene.add(dir);
  const grid = new THREE.GridHelper(12,24,0xcccccc,0xeeeeee); grid.position.y=0; scene.add(grid);

  const robot = new THREE.Group(); scene.add(robot);
  const { body, pcb } = createChassis(); robot.add(body);
  const wL = createMainWheel(.18,.08); wL.position.set(-0.42,0.16,0); robot.add(wL);
  const wR = createMainWheel(.18,.08); wR.position.set( 0.42,0.16,0); robot.add(wR);

  const orbit = { target:new THREE.Vector3(0,0.7,0), r:5.0, theta:0.8, phi:1.0, minR:2, maxR:10 };
  function applyOrbit(){
    const {r,theta,phi,target} = orbit;
    const p = new THREE.Vector3(r*Math.sin(phi)*Math.cos(theta), r*Math.cos(phi), r*Math.sin(phi)*Math.sin(theta));
    camera.position.copy(target.clone().add(p)); camera.lookAt(target);
  }
  applyOrbit();

  const el = renderer.domElement;
  let isDrag=false, btn=0, lastX=0,lastY=0;
  el.addEventListener('pointerdown', e=>{ isDrag=true; btn=e.button; lastX=e.clientX; lastY=e.clientY; el.setPointerCapture(e.pointerId); });
  el.addEventListener('pointerup', e=>{ isDrag=false; el.releasePointerCapture(e.pointerId); });
  el.addEventListener('pointerleave', ()=>{ isDrag=false; });
  el.addEventListener('contextmenu', e=> e.preventDefault());
  el.addEventListener('pointermove', e=>{
    if(!isDrag) return;
    const dx=e.clientX-lastX, dy=e.clientY-lastY; lastX=e.clientX; lastY=e.clientY;
    const rotS=0.005, panS=0.002*orbit.r;
    if(btn===2 || e.ctrlKey){
      const right=new THREE.Vector3(); camera.getWorldDirection(right); right.cross(camera.up).normalize();
      const up=camera.up.clone().normalize();
      orbit.target.addScaledVector(right, -dx*panS); orbit.target.addScaledVector(up, dy*panS);
    }else{
      orbit.theta -= dx*rotS; orbit.phi -= dy*rotS;
      const eps=0.05; orbit.phi = Math.max(eps, Math.min(Math.PI-eps, orbit.phi));
    }
    applyOrbit();
  });
  el.addEventListener('wheel', e=>{ e.preventDefault(); const k=Math.pow(1.1, e.deltaY>0?1:-1); orbit.r = Math.max(orbit.minR, Math.min(orbit.maxR, orbit.r*k)); applyOrbit(); }, {passive:false});
  window.addEventListener('resize', ()=>{ const w2 = host.clientWidth; camera.aspect=w2/h; camera.updateProjectionMatrix(); renderer.setSize(w2,h); });

  // 高度联动
  const hs = document.querySelector('#heightSlider'), hv = document.querySelector('#heightVal');
  const applyH = ()=>{ const y = parseFloat(hs.value||'0'); robot.position.y=y; hv.textContent = y.toFixed(2)+' m'; };
  hs.addEventListener('input', applyH); hs.addEventListener('change', applyH); applyH();

  state.three = { scene, camera, renderer, robot, pcb };
  setAttitude(0,0,0);
  renderer.setAnimationLoop(()=> renderer.render(scene,camera));
  appendLog('[INIT] 3D ready');
}

export function setAttitude(pitchDeg, rollDeg, yawDeg){
  const t = state.three; if (!t.robot) return;
  const r = Math.PI/180;
  const rollAdj = (rollDeg - state.attZero.roll);
  const yawAdj  = (yawDeg  - state.attZero.yaw);
  t.robot.rotation.x = pitchDeg*r;
  t.robot.rotation.z = rollAdj*r;
  t.robot.rotation.y = yawAdj*r;
  document.querySelector('#attOut').textContent =
    `pitch: ${pitchDeg.toFixed(1)}°  roll: ${rollAdj.toFixed(1)}°  yaw: ${yawAdj.toFixed(1)}°`;
}
