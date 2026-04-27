"use client";
import { useEffect, useRef, useCallback, useState } from "react";

/* ═══════════════════════════════════════════════════════════════════════════
 * Bitwise Math Library — JavaScript port of math.c
 * All functions mirror the C implementations exactly.
 * ═══════════════════════════════════════════════════════════════════════════ */

// XOR-Shift PRNG (mirrors xor_rand in math.c)
let xorState = 2463534242;
function xorRandSeed(s: number) { xorState = s !== 0 ? s >>> 0 : 2463534242; }
function xorRand(): number {
  xorState ^= (xorState << 13) >>> 0;
  xorState ^= (xorState >>> 17);
  xorState ^= (xorState << 5) >>> 0;
  return xorState >>> 0;
}

// Absolute value — branchless sign-bit mask (mirrors my_abs)
function myAbs(n: number): number {
  const mask = n >> 31;
  return (n ^ mask) - mask;
}

// Multiply via shift-and-add (mirrors my_mul)
function myMul(a: number, b: number): number {
  let neg = 0;
  if (a < 0) { a = myAbs(a); neg ^= 1; }
  if (b < 0) { b = myAbs(b); neg ^= 1; }
  let result = 0;
  while (b > 0) {
    if (b & 1) result += a;
    a <<= 1; b >>= 1;
  }
  return neg ? -result : result;
}

// Division via long division (mirrors my_div)
function myDiv(a: number, b: number): number {
  if (b === 0) return 0;
  let neg = 0;
  if (a < 0) { a = myAbs(a); neg ^= 1; }
  if (b < 0) { b = myAbs(b); neg ^= 1; }
  let quotient = 0, remainder = 0;
  for (let i = 30; i >= 0; i--) {
    remainder <<= 1;
    remainder |= (a >> i) & 1;
    if (remainder >= b) { remainder -= b; quotient |= (1 << i); }
  }
  return neg ? -quotient : quotient;
}

// Modulo (mirrors my_mod)
function myMod(a: number, b: number): number {
  if (b === 0) return 0;
  return a - myMul(myDiv(a, b), b);
}

// Clamp (mirrors my_clamp)
function myClamp(v: number, lo: number, hi: number): number {
  if (v < lo) return lo;
  if (v > hi) return hi;
  return v;
}

/* ═══════════════════════════════════════════════════════════════════════════
 * Game Constants
 * ═══════════════════════════════════════════════════════════════════════════ */
const CELL      = 18;    // px per grid cell
const COLS      = 32;
const ROWS      = 20;
const MAX_LEN   = 256;
const BASE_MS   = 150;   // base game speed ms
const SPEED_STEP= 10;    // speed increase per level

type Point = { x: number; y: number };
type Dir   = { dx: number; dy: number };
type Phase = "idle" | "running" | "over";

interface GameState {
  body:      Point[];
  dir:       Dir;
  foods:     Point[];
  obstacles: Point[];
  score:     number;
  highScore: number;
  len:       number;
  level:     number;
  foodsEaten: number;
  phase:     Phase;
  memUsed:   number;
  allocCount: number;
}

interface LogEntry {
  ts: number;
  msg: string;
  type: "info" | "alloc" | "free" | "error" | "level";
}

interface SnakeCanvasProps {
  onLog:   (entry: LogEntry) => void;
  onStats: (s: { score: number; highScore: number; level: number; len: number; memUsed: number }) => void;
}

/* ═══════════════════════════════════════════════════════════════════════════
 * SnakeCanvas Component
 * ═══════════════════════════════════════════════════════════════════════════ */
export default function SnakeCanvas({ onLog, onStats }: SnakeCanvasProps) {
  const canvasRef  = useRef<HTMLCanvasElement>(null);
  const stateRef   = useRef<GameState | null>(null);
  const loopRef    = useRef<ReturnType<typeof setTimeout> | null>(null);
  const [phase, setPhase] = useState<Phase>("idle");

  // ── Logging helper ─────────────────────────────────────────────────────
  const log = useCallback((msg: string, type: LogEntry["type"] = "info") => {
    onLog({ ts: Date.now(), msg, type });
  }, [onLog]);

  // ── Spawn food (uses xorRand + myMod — bitwise) ─────────────────────────
  const spawnFoodAt = useCallback((body: Point[], obstacles: Point[], currentFoods: Point[], index: number): Point => {
    let fx: number, fy: number, occupied: boolean;
    let attempts = 0;
    do {
      fx = 1 + myMod(xorRand(), COLS - 2);
      fy = 1 + myMod(xorRand(), ROWS - 2);
      occupied = body.some(s => s.x === fx && s.y === fy) ||
                 obstacles.some(o => o.x === fx && o.y === fy) ||
                 currentFoods.some((f, i) => i !== index && f.x === fx && f.y === fy);
      attempts++;
    } while (occupied && attempts < 100);
    return { x: fx, y: fy };
  }, []);

  const spawnObstacles = useCallback((): Point[] => {
    const obs: Point[] = [];
    const startX = myDiv(COLS, 2);
    const startY = myDiv(ROWS, 2);
    for (let i = 0; i < 5; i++) {
      let ox: number, oy: number, occupied: boolean;
      do {
        ox = 1 + myMod(xorRand(), COLS - 2);
        oy = 1 + myMod(xorRand(), ROWS - 2);
        occupied = myAbs(ox - startX) < 5 && myAbs(oy - startY) < 5 ||
                   obs.some(o => o.x === ox && o.y === oy);
      } while (occupied);
      obs.push({ x: ox, y: oy });
    }
    return obs;
  }, []);

  const initGame = useCallback((): GameState => {
    xorRandSeed(Date.now() >>> 0);
    const startX = myDiv(COLS, 2);
    const startY = myDiv(ROWS, 2);
    const body: Point[] = [
      { x: startX,     y: startY },
      { x: startX - 1, y: startY },
      { x: startX - 2, y: startY },
    ];
    const obstacles = spawnObstacles();
    const foods: Point[] = [];
    for (let i = 0; i < 3; i++) {
      foods.push(spawnFoodAt(body, obstacles, foods, i));
    }
    return {
      body, dir: { dx: 1, dy: 0 }, foods, obstacles,
      score: 0, highScore: stateRef.current?.highScore || 0, len: 3, level: 1, foodsEaten: 0,
      phase: "running", memUsed: 8 * MAX_LEN + 24, allocCount: 1,
    };
  }, [spawnFoodAt, spawnObstacles]);

  // ── Game speed from level (mirrors game_speed_us in main.c) ───────────
  const gameSpeedMs = (level: number) => {
    const step  = myMul(level - 1, SPEED_STEP);
    return myClamp(BASE_MS - step, 60, 200);
  };

  // ── Draw game frame ────────────────────────────────────────────────────
  const draw = useCallback(() => {
    const canvas = canvasRef.current;
    const gs     = stateRef.current;
    if (!canvas || !gs) return;
    const ctx = canvas.getContext("2d")!;
    const W = COLS * CELL, H = ROWS * CELL;

    // Background
    ctx.fillStyle = "#04060a";
    ctx.fillRect(0, 0, W, H);

    // Grid lines (subtle)
    ctx.strokeStyle = "rgba(0, 255, 136, 0.04)";
    ctx.lineWidth = 0.5;
    for (let x = 0; x <= COLS; x++) {
      ctx.beginPath(); ctx.moveTo(x * CELL, 0); ctx.lineTo(x * CELL, H); ctx.stroke();
    }
    for (let y = 0; y <= ROWS; y++) {
      ctx.beginPath(); ctx.moveTo(0, y * CELL); ctx.lineTo(W, y * CELL); ctx.stroke();
    }

    // Border wall
    ctx.strokeStyle = "#00ff88";
    ctx.lineWidth = 2;
    ctx.shadowColor = "#00ff88";
    ctx.shadowBlur  = 8;
    ctx.strokeRect(1, 1, W - 2, H - 2);
    ctx.shadowBlur = 0;

    if (gs.phase === "idle") {
      ctx.fillStyle = "#00ff88";
      ctx.font = `bold 22px JetBrains Mono, monospace`;
      ctx.textAlign = "center";
      ctx.fillText("SNAKE OS v2.1", W / 2, H / 2 - 20);
      ctx.font = `14px JetBrains Mono, monospace`;
      ctx.fillStyle = "#8aa898";
      ctx.fillText("Press SPACE or click START", W / 2, H / 2 + 10);
      ctx.fillText("WASD / Arrow Keys to move", W / 2, H / 2 + 32);
      ctx.textAlign = "left";
      return;
    }

    // Obstacles
    ctx.fillStyle = "#ff4466";
    ctx.shadowColor = "#ff4466"; ctx.shadowBlur = 15;
    gs.obstacles.forEach(o => {
      ctx.fillRect(o.x * CELL + 2, o.y * CELL + 2, CELL - 4, CELL - 4);
    });

    // Food
    ctx.fillStyle = "#ffdd44";
    ctx.shadowColor = "#ffdd44"; ctx.shadowBlur = 20;
    gs.foods.forEach(f => {
      ctx.beginPath();
      ctx.arc(f.x * CELL + CELL/2, f.y * CELL + CELL/2, CELL/2 - 2, 0, Math.PI * 2);
      ctx.fill();
    });

    // Snake body
    ctx.shadowBlur = 10;
    gs.body.forEach((seg, i) => {
      const bx = seg.x * CELL + 1, by = seg.y * CELL + 1;
      const sz = CELL - 2;
      if (i === 0) {
        // Head
        ctx.fillStyle = "#00ff88";
        ctx.shadowColor = "#00ff88"; ctx.shadowBlur = 16;
      } else {
        const t = 1 - i / gs.len;
        const g = Math.round(100 + 155 * t);
        ctx.fillStyle = `rgb(0, ${g}, ${Math.round(g * 0.5)})`;
        ctx.shadowBlur = 0;
      }
      ctx.beginPath();
      ctx.roundRect(bx, by, sz, sz, i === 0 ? 5 : 3);
      ctx.fill();
    });
    ctx.shadowBlur = 0;

    // Game over overlay
    if (gs.phase === "over") {
      ctx.fillStyle = "rgba(4, 6, 10, 0.88)";
      ctx.fillRect(0, 0, W, H);
      ctx.fillStyle = "#ff4466";
      ctx.font = "bold 26px JetBrains Mono, monospace";
      ctx.textAlign = "center";
      ctx.fillText("PROCESS EXCEPTION", W/2, H/2 - 48);
      ctx.fillStyle = "#8aa898";
      ctx.font = "13px JetBrains Mono, monospace";
      ctx.fillText("Segmentation fault: Snake collided", W/2, H/2 - 20);
      ctx.fillStyle = "#00ff88";
      ctx.font = "bold 20px JetBrains Mono, monospace";
      ctx.fillText(`Score: ${gs.score}`, W/2, H/2 + 16);
      ctx.fillStyle = "#8aa898";
      ctx.font = "13px JetBrains Mono, monospace";
      ctx.fillText("Press SPACE to restart", W/2, H/2 + 50);
      ctx.textAlign = "left";
    }
  }, []);

  // ── Game tick ──────────────────────────────────────────────────────────
  const tick = useCallback(() => {
    const gs = stateRef.current;
    if (!gs || gs.phase !== "running") return;

    const newHead: Point = {
      x: gs.body[0].x + gs.dir.dx,
      y: gs.body[0].y + gs.dir.dy,
    };

    // Obstacle or Wall collision
    if (newHead.x <= 0 || newHead.x >= COLS - 1 ||
        newHead.y <= 0 || newHead.y >= ROWS - 1 ||
        gs.obstacles.some(o => o.x === newHead.x && o.y === newHead.y)) {
      gs.phase = "over";
      setPhase("over");
      log(`[snake.c] collision detected!`, "error");
      draw();
      onStats({ score: gs.score, highScore: gs.highScore, level: gs.level, len: gs.len, memUsed: gs.memUsed });
      return;
    }

    // Self collision
    for (let i = 1; i < gs.body.length; i++) {
      if (newHead.x === gs.body[i].x && newHead.y === gs.body[i].y) {
        gs.phase = "over";
        setPhase("over");
        log(`[snake.c] self-collision!`, "error");
        draw();
        onStats({ score: gs.score, highScore: gs.highScore, level: gs.level, len: gs.len, memUsed: gs.memUsed });
        return;
      }
    }

    // Food eaten?
    let ateFoodIdx = -1;
    for (let i = 0; i < gs.foods.length; i++) {
        if (newHead.x === gs.foods[i].x && newHead.y === gs.foods[i].y) {
            ateFoodIdx = i; break;
        }
    }
    const ateFood = ateFoodIdx !== -1;
    const newBody = ateFood
      ? [newHead, ...gs.body]
      : [newHead, ...gs.body.slice(0, -1)];

    let newScore = gs.score;
    let newHi    = gs.highScore;
    let newLen   = gs.len;
    let newLevel = gs.level;
    let newFoods = gs.foodsEaten;
    const newFoodArr = [...gs.foods];
    let newMem   = gs.memUsed;

    if (ateFood) {
      newScore += myMul(10, gs.level);
      if (newScore > newHi) newHi = newScore;
      newFoods++;
      newLen = Math.min(newLen + 1, MAX_LEN);
      newMem += 8;
      if (myMod(newFoods, 5) === 0) newLevel++;
      newFoodArr[ateFoodIdx] = spawnFoodAt(newBody, gs.obstacles, newFoodArr, ateFoodIdx);
      log(`[snake.c] food eaten! score: ${newScore}`, "info");
    }

    stateRef.current = {
      body: newBody, dir: gs.dir, foods: newFoodArr, obstacles: gs.obstacles,
      score: newScore, highScore: newHi, len: newLen, level: newLevel,
      foodsEaten: newFoods, phase: "running",
      memUsed: newMem, allocCount: gs.allocCount,
    };

    draw();
    onStats({ score: newScore, highScore: newHi, level: newLevel, len: newLen, memUsed: newMem });

    // Schedule next tick with adaptive speed
    loopRef.current = setTimeout(tick, gameSpeedMs(newLevel));
  }, [draw, log, onStats, spawnFoodAt]);

  // ── Start / Restart ───────────────────────────────────────────────────
  const startGame = useCallback(() => {
    if (loopRef.current) clearTimeout(loopRef.current);
    log("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━", "info");
    log("[main.c]   SNAKE OS booting...", "info");
    log("[keyboard.c] keyboard_init() → raw non-blocking mode", "info");
    log("[memory.c] mem_reset() → virtual_ram[65536] cleared", "info");
    const gs = initGame();
    stateRef.current = gs;
    setPhase("running");
    draw();
    loopRef.current = setTimeout(tick, gameSpeedMs(gs.level));
  }, [initGame, draw, tick, log]);

  // ── Keyboard input ────────────────────────────────────────────────────
  useEffect(() => {
    const onKey = (e: KeyboardEvent) => {
      const gs = stateRef.current;
      if (!gs) { if (e.code === "Space") startGame(); return; }

      if (gs.phase === "idle" || gs.phase === "over") {
        if (e.code === "Space") startGame();
        return;
      }

      const { dx, dy } = gs.dir;
      if ((e.key === "w" || e.key === "W" || e.key === "ArrowUp")    && dy !== 1)  gs.dir = { dx: 0,  dy: -1 };
      if ((e.key === "s" || e.key === "S" || e.key === "ArrowDown")  && dy !== -1) gs.dir = { dx: 0,  dy:  1 };
      if ((e.key === "a" || e.key === "A" || e.key === "ArrowLeft")  && dx !== 1)  gs.dir = { dx: -1, dy:  0 };
      if ((e.key === "d" || e.key === "D" || e.key === "ArrowRight") && dx !== -1) gs.dir = { dx:  1, dy:  0 };
      if (e.code === "Space" && gs.phase === "over") startGame();
      e.preventDefault();
    };
    window.addEventListener("keydown", onKey);
    return () => window.removeEventListener("keydown", onKey);
  }, [startGame]);

  // ── Cleanup on unmount ────────────────────────────────────────────────
  useEffect(() => {
    draw();
    return () => { if (loopRef.current) clearTimeout(loopRef.current); };
  }, [draw]);

  return (
    <div style={{ display: "flex", flexDirection: "column", gap: 12 }}>
      <canvas
        ref={canvasRef}
        width={COLS * CELL}
        height={ROWS * CELL}
        style={{
          display: "block",
          borderRadius: 8,
          border: "1px solid #1a2a40",
          cursor: "crosshair",
          maxWidth: "100%",
        }}
        onClick={() => { if (!stateRef.current || stateRef.current.phase !== "running") startGame(); }}
      />
      <div style={{ display: "flex", gap: 8 }}>
        <button
          className="btn btn-green"
          onClick={startGame}
          style={{ flex: 1 }}
          id="btn-start-game"
        >
          ▶ {phase === "running" ? "RESTART" : "START GAME"}
        </button>
        <span className="tag tag-green" style={{ alignSelf: "center" }}>
          WASD / ↑↓←→
        </span>
      </div>
    </div>
  );
}
