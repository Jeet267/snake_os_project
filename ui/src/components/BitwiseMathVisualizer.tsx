"use client";
import { useState, useEffect, useCallback } from "react";

/* ═══════════════════════════════════════════════════════════════════════════
 * Bitwise Math Visualizer
 * Animates the Russian Peasant (shift-and-add) multiplication algorithm.
 * ═══════════════════════════════════════════════════════════════════════════ */

export default function BitwiseMathVisualizer() {
  const [a, setA] = useState(5);
  const [b, setB] = useState(3);
  const [steps, setSteps] = useState<{ a: number; b: number; adding: boolean; result: number }[]>([]);
  const [currentStep, setCurrentStep] = useState(-1);
  const [isAnimating, setIsAnimating] = useState(false);

  const calculateSteps = useCallback(() => {
    let tempA = a;
    let tempB = b;
    let res = 0;
    const newSteps = [];

    while (tempB > 0) {
      const adding = (tempB & 1) === 1;
      if (adding) res += tempA;
      newSteps.push({ a: tempA, b: tempB, adding, result: res });
      tempA <<= 1;
      tempB >>= 1;
    }
    setSteps(newSteps);
    setCurrentStep(-1);
  }, [a, b]);

  useEffect(() => {
    calculateSteps();
  }, [calculateSteps]);

  const nextStep = () => {
    if (currentStep < steps.length - 1) {
      setCurrentStep(s => s + 1);
    } else {
      setIsAnimating(false);
    }
  };

  useEffect(() => {
    if (isAnimating) {
      const timer = setTimeout(nextStep, 1000);
      return () => clearTimeout(timer);
    }
  }, [isAnimating, currentStep, steps.length]);

  const startAnimation = () => {
    setCurrentStep(-1);
    setIsAnimating(true);
  };

  return (
    <div style={{ display: "flex", flexDirection: "column", gap: 16, padding: "0 18px 20px" }}>
      <div style={{ display: "flex", gap: 12, alignItems: "center" }}>
        <div style={{ display: "flex", flexDirection: "column", gap: 4 }}>
          <label className="stat-label">Value A</label>
          <input
            type="number"
            value={a}
            onChange={(e) => setA(parseInt(e.target.value) || 0)}
            style={{
              background: "var(--bg-panel2)",
              border: "1px solid var(--border)",
              color: "var(--neon-green)",
              padding: "4px 8px",
              borderRadius: 4,
              width: 60,
              fontFamily: "var(--font-mono)",
            }}
          />
        </div>
        <span style={{ color: "var(--text-dim)", paddingTop: 16 }}>×</span>
        <div style={{ display: "flex", flexDirection: "column", gap: 4 }}>
          <label className="stat-label">Value B</label>
          <input
            type="number"
            value={b}
            onChange={(e) => setB(parseInt(e.target.value) || 0)}
            style={{
              background: "var(--bg-panel2)",
              border: "1px solid var(--border)",
              color: "var(--neon-blue)",
              padding: "4px 8px",
              borderRadius: 4,
              width: 60,
              fontFamily: "var(--font-mono)",
            }}
          />
        </div>
        <button
          className="btn btn-blue"
          style={{ marginTop: 18, height: 32, padding: "0 12px" }}
          onClick={startAnimation}
          disabled={isAnimating}
        >
          {isAnimating ? "RUNNING..." : "VISUALIZE"}
        </button>
      </div>

      <div style={{ display: "flex", flexDirection: "column", gap: 8 }}>
        <div
          style={{
            display: "grid",
            gridTemplateColumns: "1fr 1fr 1fr 1fr",
            padding: "8px",
            borderBottom: "1px solid var(--border)",
            fontFamily: "var(--font-mono)",
            fontSize: 11,
            color: "var(--text-dim)",
          }}
        >
          <span>A (Shift Left)</span>
          <span>B (Shift Right)</span>
          <span>Action</span>
          <span>Result</span>
        </div>

        <div style={{ display: "flex", flexDirection: "column", gap: 4, height: 120, overflowY: "auto" }}>
          {steps.map((step, i) => (
            <div
              key={i}
              style={{
                display: "grid",
                gridTemplateColumns: "1fr 1fr 1fr 1fr",
                padding: "6px 8px",
                borderRadius: 4,
                fontFamily: "var(--font-mono)",
                fontSize: 12,
                background: i === currentStep ? "rgba(0, 255, 136, 0.1)" : "transparent",
                border: i === currentStep ? "1px solid rgba(0, 255, 136, 0.2)" : "1px solid transparent",
                opacity: i <= currentStep ? 1 : 0.3,
                transition: "all 0.3s ease",
              }}
            >
              <span style={{ color: "var(--neon-green)" }}>{step.a} <span style={{fontSize: 10, color: "var(--text-dim)"}}>({step.a.toString(2).padStart(8, '0')})</span></span>
              <span style={{ color: "var(--neon-blue)" }}>{step.b} <span style={{fontSize: 10, color: "var(--text-dim)"}}>({step.b.toString(2).padStart(8, '0')})</span></span>
              <span style={{ color: step.adding ? "var(--neon-yellow)" : "var(--text-dim)" }}>
                {step.adding ? "+ Add A" : "Skip"}
              </span>
              <span style={{ fontWeight: 600 }}>{i <= currentStep ? step.result : "?"}</span>
            </div>
          ))}
          {steps.length === 0 && <div style={{textAlign: 'center', color: 'var(--text-dim)', fontSize: 12, paddingTop: 40}}>Enter values to begin</div>}
        </div>
      </div>

      {currentStep === steps.length - 1 && steps.length > 0 && (
        <div className="fade-in" style={{ padding: "12px", background: "rgba(0, 255, 136, 0.05)", border: "1px solid var(--neon-green)", borderRadius: 6, textAlign: "center" }}>
          <span style={{ color: "var(--text-secondary)", fontSize: 12 }}>Final Result:</span>
          <span style={{ color: "var(--neon-green)", fontWeight: 700, fontSize: 18, marginLeft: 8 }}>{steps[steps.length - 1].result}</span>
        </div>
      )}
    </div>
  );
}
