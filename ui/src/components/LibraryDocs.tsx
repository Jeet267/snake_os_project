"use client";
import { useState } from "react";

const LIBS = [
  {
    id: "math",
    name: "math.c",
    color: "var(--neon-green)",
    tagClass: "tag-green",
    desc: "Pure bitwise arithmetic — zero use of *, /, % operators",
    functions: [
      { name: "xor_rand()", technique: "XOR-Shift (x ^= x<<13; x ^= x>>17; x ^= x<<5)", returns: "unsigned int" },
      { name: "my_mul(a, b)", technique: "Russian peasant: shift-and-add (while b>0: if b&1 add a; a<<=1; b>>=1)", returns: "int" },
      { name: "my_div(a, b)", technique: "Non-restoring long division: bit-by-bit from MSB", returns: "int" },
      { name: "my_mod(a, b)", technique: "a - my_mul(my_div(a,b), b)", returns: "int" },
      { name: "my_abs(x)", technique: "Branchless sign mask: mask = x>>31; (x^mask)-mask", returns: "int" },
      { name: "my_clamp(v,lo,hi)", technique: "Conditional comparisons using sign results", returns: "int" },
      { name: "my_pow2(n)", technique: "Left shift: 1 << n", returns: "int" },
      { name: "my_sqrt(n)", technique: "Binary search on bits from MSB to LSB", returns: "int" },
      { name: "my_log2(n)", technique: "Count right-shifts until n==1", returns: "int" },
    ],
    snippet: `/* my_mul — shift-and-add (Russian peasant) */
int my_mul(int a, int b) {
    int result = 0;
    /* handle sign separately */
    int neg = 0;
    if (a < 0) { a = my_abs(a); neg ^= 1; }
    if (b < 0) { b = my_abs(b); neg ^= 1; }
    while (b > 0) {
        if (b & 1)   result += a;  /* add if LSB set */
        a <<= 1;                    /* double a       */
        b >>= 1;                    /* halve  b       */
    }
    return neg ? -result : result;
}`,
  },
  {
    id: "memory",
    name: "memory.c",
    color: "var(--neon-blue)",
    tagClass: "tag-blue",
    desc: "64KB virtual RAM with bump allocator + free-list",
    functions: [
      { name: "my_alloc(size)", technique: "First-fit free-list, then bump pointer into virtual_ram[65536]", returns: "void*" },
      { name: "my_dealloc(ptr)", technique: "Validates HDR_MAGIC sentinel, adds to free-list head", returns: "void" },
      { name: "mem_reset()", technique: "Zero-fills virtual_ram, resets bump + free-list", returns: "void" },
      { name: "mem_bytes_used()", technique: "Returns vram_top (bump pointer position)", returns: "int" },
      { name: "mem_alloc_count()", technique: "Tracks live allocations via alloc_count counter", returns: "int" },
    ],
    snippet: `/* 64KB static virtual address space */
#define VRAM_SIZE 65536
static char virtual_ram[VRAM_SIZE];
static int  vram_top = 0;

void *my_alloc(int size) {
    int aligned = align_up(size);   /* 4-byte aligned */
    /* 1. Check free-list (first-fit) */
    BlockHeader *cur = free_list;
    while (cur) {
        if (cur->size >= aligned) {
            cur->free = 0; alloc_count++;
            return (void*)((char*)cur + HDR_SIZE);
        }
        cur = cur->next_free;
    }
    /* 2. Bump allocate */
    BlockHeader *hdr = (BlockHeader*)(virtual_ram + vram_top);
    hdr->magic = HDR_MAGIC; hdr->size = aligned;
    vram_top += HDR_SIZE + aligned;
    return (void*)((char*)hdr + HDR_SIZE);
}`,
  },
  {
    id: "string",
    name: "string.c",
    color: "var(--neon-purple)",
    tagClass: "tag-purple",
    desc: "String manipulation without <string.h>",
    functions: [
      { name: "my_strlen(s)", technique: "Walk pointer until '\\0'; return pointer difference", returns: "int" },
      { name: "my_strcpy(dst, src)", technique: "Copy chars via *dst++ = *src++ until null", returns: "char*" },
      { name: "my_strcmp(a, b)", technique: "Walk both until mismatch; return char difference", returns: "int" },
      { name: "my_strcat(dst, src)", technique: "Walk to end of dst, then copy src", returns: "char*" },
      { name: "my_int_to_str(n, buf)", technique: "Extract digits via my_mod(n,10), reverse; replaces sprintf", returns: "char*" },
      { name: "my_str_to_int(s)", technique: "Walk chars, accumulate via my_mul(acc,10) + digit", returns: "int" },
    ],
    snippet: `/* my_int_to_str — uses bitwise my_mod/my_div */
char *my_int_to_str(int n, char *buf) {
    char tmp[24]; int i = 0, neg = 0;
    if (n == 0) { buf[0]='0'; buf[1]='\0'; return buf; }
    if (n < 0)  { neg = 1; n = my_abs(n); }
    while (n > 0) {
        tmp[i++] = '0' + (char)my_mod(n, 10); /* LSB digit */
        n = my_div(n, 10);
    }
    if (neg) tmp[i++] = '-';
    /* reverse into buf */
    int idx = 0;
    while (i > 0) buf[idx++] = tmp[--i];
    buf[idx] = '\0';
    return buf;
}`,
  },
  {
    id: "screen",
    name: "screen.c",
    color: "var(--neon-orange)",
    tagClass: "tag-orange",
    desc: "Flicker-free framebuffer diff-renderer",
    functions: [
      { name: "screen_clear()", technique: "Fill back buffer with spaces (no terminal I/O yet)", returns: "void" },
      { name: "screen_draw_char(x,y,c)", technique: "Write to fb[y][x] back buffer", returns: "void" },
      { name: "screen_draw_string(x,y,s)", technique: "Walk string, call screen_draw_char per char", returns: "void" },
      { name: "screen_flush()", technique: "Diff back vs front buffer; only emit ANSI for changed cells", returns: "void" },
      { name: "screen_hard_clear()", technique: "Send \\033[2J\\033[H; set fb_dirty=1", returns: "void" },
    ],
    snippet: `/* Diff-flush: only redraw changed cells → no flicker */
void screen_flush(void) {
    int x, y;
    for (y = 0; y < SCREEN_FB_HEIGHT; y++) {
        for (x = 0; x < SCREEN_FB_WIDTH; x++) {
            if (fb_dirty || fb[y][x] != prev_fb[y][x]) {
                screen_move_cursor(x, y);
                putchar(fb[y][x]);
                prev_fb[y][x] = fb[y][x];
            }
        }
    }
    fflush(stdout);
    fb_dirty = 0;
}`,
  },
  {
    id: "keyboard",
    name: "keyboard.c",
    color: "var(--neon-red)",
    tagClass: "tag-red",
    desc: "Non-blocking raw terminal keyboard driver",
    functions: [
      { name: "keyboard_init()", technique: "tcgetattr → disable ICANON|ECHO → fcntl O_NONBLOCK", returns: "void" },
      { name: "read_key()", technique: "getchar() non-blocking; parse ESC [A/B/C/D for arrow keys", returns: "int" },
      { name: "keyboard_restore()", technique: "tcsetattr restores saved termios; fcntl resets flags", returns: "void" },
    ],
    snippet: `void keyboard_init(void) {
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    /* Raw mode: disable canonical + echo */
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    /* Non-blocking I/O */
    oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);
}`,
  },
];

export default function LibraryDocs() {
  const [active, setActive] = useState("math");
  const lib = LIBS.find(l => l.id === active)!;

  return (
    <div style={{ display: "flex", flexDirection: "column", minHeight: 0 }}>
      <div className="tabs">
        {LIBS.map(l => (
          <button
            key={l.id}
            className={`tab ${active === l.id ? "active" : ""}`}
            onClick={() => setActive(l.id)}
            id={`lib-tab-${l.id}`}
            style={active === l.id ? { color: l.color, borderBottomColor: l.color } : {}}
          >
            {l.name}
          </button>
        ))}
      </div>

      <div style={{ padding: "20px 18px", display: "flex", flexDirection: "column", gap: 16, overflowY: "auto" }}>
        <div>
          <span className={`tag ${lib.tagClass}`}>{lib.name}</span>
          <p style={{ marginTop: 8, fontSize: 13, color: "var(--text-secondary)" }}>{lib.desc}</p>
        </div>

        <table style={{ width: "100%", borderCollapse: "collapse", fontSize: 12, fontFamily: "var(--font-mono)" }}>
          <thead>
            <tr style={{ borderBottom: "1px solid var(--border)" }}>
              <th style={{ textAlign: "left", padding: "6px 8px", color: "var(--text-dim)", fontWeight: 600 }}>Function</th>
              <th style={{ textAlign: "left", padding: "6px 8px", color: "var(--text-dim)", fontWeight: 600 }}>Technique</th>
            </tr>
          </thead>
          <tbody>
            {lib.functions.map((fn, i) => (
              <tr key={i} style={{ borderBottom: "1px solid #0d1820" }}>
                <td style={{ padding: "8px 8px", color: lib.color, whiteSpace: "nowrap" }}>{fn.name}</td>
                <td style={{ padding: "8px 8px", color: "var(--text-secondary)", lineHeight: 1.5 }}>{fn.technique}</td>
              </tr>
            ))}
          </tbody>
        </table>

        <div>
          <div style={{ fontSize: 11, color: "var(--text-dim)", fontFamily: "var(--font-mono)", marginBottom: 6, textTransform: "uppercase", letterSpacing: 1 }}>Source</div>
          <pre className="code-block" style={{ fontSize: 11 }}>{lib.snippet}</pre>
        </div>
      </div>
    </div>
  );
}
