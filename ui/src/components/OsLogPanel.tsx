"use client";
import { useEffect, useRef } from "react";

interface LogEntry {
  ts: number;
  msg: string;
  type: "info" | "alloc" | "free" | "error" | "level";
}
interface OsLogPanelProps { logs: LogEntry[]; }

const TYPE_COLORS = { info: "var(--text-secondary)", alloc: "var(--neon-green)", free: "var(--neon-blue)", error: "var(--neon-red)", level: "var(--neon-purple)" };
const TYPE_PREFIX = { info: "INFO ", alloc: "ALLOC", free: "FREE ", error: "ERR  ", level: "LEVEL" };

export default function OsLogPanel({ logs }: OsLogPanelProps) {
  const bottomRef = useRef<HTMLDivElement>(null);
  useEffect(() => { bottomRef.current?.scrollIntoView({ behavior: "smooth" }); }, [logs]);

  const fmt = (ts: number) => {
    const d = new Date(ts);
    return [d.getHours(), d.getMinutes(), d.getSeconds()].map(n => String(n).padStart(2,"0")).join(":")
      + "." + String(d.getMilliseconds()).padStart(3,"0");
  };

  return (
    <div style={{ padding: "12px 16px", height: 280, overflowY: "auto", fontFamily: "var(--font-mono)", fontSize: 11, lineHeight: 1.7 }}>
      {logs.length === 0 && (
        <div style={{ color: "var(--text-dim)", textAlign: "center", paddingTop: 80 }}>
          — kernel idle —<span className="blink">█</span>
        </div>
      )}
      {logs.map((entry, i) => (
        <div key={i} style={{ display: "flex", gap: 10 }}>
          <span style={{ color: "var(--text-dim)", whiteSpace: "nowrap", flexShrink: 0 }}>{fmt(entry.ts)}</span>
          <span style={{ color: TYPE_COLORS[entry.type], whiteSpace: "nowrap", flexShrink: 0, fontWeight: 600 }}>[{TYPE_PREFIX[entry.type]}]</span>
          <span style={{ color: "var(--text-primary)", wordBreak: "break-all" }}>{entry.msg}</span>
        </div>
      ))}
      <div ref={bottomRef} />
    </div>
  );
}
