"use client";
import { useState, useCallback } from "react";
import SnakeCanvas from "@/components/SnakeCanvas";

export default function Home() {
  const [stats, setStats] = useState({
    score: 0,
    highScore: 0,
    level: 1,
    len: 3,
    memUsed: 0
  });

  const handleStats = useCallback((s: any) => {
    setStats(s);
  }, []);

  return (
    <div className="page-wrapper" style={{ background: "var(--bg-void)", minHeight: "100vh" }}>
      <div className="crt-overlay" />
      
      <main className="fade-in" style={{ width: "100%", maxWidth: "1200px", margin: "0 auto", padding: "40px 20px", display: "flex", flexDirection: "column", gap: 32 }}>
        
        {/* Header Section */}
        <header style={{ display: "flex", justifyContent: "space-between", alignItems: "center" }}>
          <div>
            <h1 className="neon-glow-green mono" style={{ fontSize: 32, margin: 0, letterSpacing: 4 }}>SNAKE_OS_v2.1</h1>
            <p className="dim mono" style={{ fontSize: 12, marginTop: 4 }}>KERNEL_STATUS: OPTIMIZED_RUNNING</p>
          </div>
          
          <div style={{ display: "flex", gap: 20 }}>
             <div className="stat-card" style={{ minWidth: 140 }}>
                <div className="stat-label">HI_SCORE</div>
                <div className="stat-value yellow">{stats.highScore}</div>
             </div>
             <div className="stat-card" style={{ minWidth: 140 }}>
                <div className="stat-label">SCORE</div>
                <div className="stat-value">{stats.score}</div>
             </div>
          </div>
        </header>

        {/* Main Content Grid */}
        <div style={{ display: "grid", gridTemplateColumns: "1fr 340px", gap: 32 }}>
          
          {/* Game Canvas Panel */}
          <section className="glass" style={{ padding: 24 }}>
            <div className="panel-header" style={{ marginBottom: 16 }}>
              <div className="dot" />
              <span>PRIMARY_DISPLAY_BUFFER</span>
            </div>
            <SnakeCanvas onLog={() => {}} onStats={handleStats} />
          </section>

          {/* Right Info Panel */}
          <aside style={{ display: "flex", flexDirection: "column", gap: 24 }}>
             
             <div className="stat-card">
                <div className="stat-label">OS_LEVEL</div>
                <div className="stat-value purple">{stats.level}</div>
                <div className="progress-bar" style={{ marginTop: 12 }}>
                   <div className="progress-fill" style={{ width: `${(stats.level % 10) * 10}%` }} />
                </div>
             </div>

             <div className="stat-card">
                <div className="stat-label">SNAKE_SEGMENTS</div>
                <div className="stat-value blue">{stats.len}</div>
                <div className="stat-sub">MAX_ALLOC: 256</div>
             </div>

             <div className="glass" style={{ padding: 20, flex: 1 }}>
                <div className="panel-header" style={{ padding: "0 0 12px 0", fontSize: 11 }}>
                   <div className="dot blue" />
                   <span>SYSTEM_LOG</span>
                </div>
                <div className="mono" style={{ fontSize: 11, color: "var(--text-secondary)", lineHeight: 1.8 }}>
                   <p>&gt; BOOTING KERNEL...</p>
                   <p>&gt; MAPPING VIRTUAL_RAM...</p>
                   <p>&gt; ATTACHING INTERRUPTS...</p>
                   <p style={{ color: "var(--neon-green)" }}>&gt; STATUS: OK</p>
                   <p style={{ marginTop: 8 }}>MEM_USAGE: {stats.memUsed}B</p>
                   <p>CPU_LOAD: {stats.level * 8}%</p>
                </div>
             </div>

          </aside>
        </div>

        {/* Footer */}
        <footer className="mono" style={{ textAlign: "center", color: "var(--text-dim)", fontSize: 11, borderTop: "1px solid var(--border)", paddingTop: 20 }}>
           POWERED BY CUSTOM C LIBRARIES: MATH.C | MEMORY.C | SCREEN.C | STRING.C
        </footer>

      </main>
    </div>
  );
}
