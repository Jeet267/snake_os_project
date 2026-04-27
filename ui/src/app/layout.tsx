import type { Metadata } from "next";
import "./globals.css";

export const metadata: Metadata = {
  title: "Snake OS — Systems Programming Capstone Dashboard",
  description:
    "Interactive dashboard for the Snake OS capstone: custom C libraries built with bitwise math, virtual memory, and real-time terminal rendering. Includes a browser-playable snake game and library documentation.",
};

export default function RootLayout({
  children,
}: {
  children: React.ReactNode;
}) {
  return (
    <html lang="en">
      <head>
        <link
          href="https://fonts.googleapis.com/css2?family=JetBrains+Mono:wght@300;400;500;600;700&family=Inter:wght@300;400;500;600;700&display=swap"
          rel="stylesheet"
        />
      </head>
      <body>{children}</body>
    </html>
  );
}
