#!/usr/bin/env python3
"""Converts a `tmux capture-pane -e -p` dump into a VGA-styled HTML page."""
import html, re, sys

# Classic VGA text-mode palette: normal and bright (bold) variants.
NORMAL = ['#000000', '#AA0000', '#00AA00', '#AA5500', '#0000AA', '#AA00AA', '#00AAAA', '#AAAAAA']
BRIGHT = ['#555555', '#FF5555', '#55FF55', '#FFFF55', '#5555FF', '#FF55FF', '#55FFFF', '#FFFFFF']
SGR = re.compile(r'\x1b\[([0-9;]*)m')
COLS, ROWS = 80, 24

def render(text, title):
    rows = []
    fg, bg, bold = 7, 0, False  # tmux carries SGR state across lines.
    for line in text.split('\n')[:ROWS]:
        cells, pos = [], 0
        for m in SGR.finditer(line):
            for ch in line[pos:m.start()]:
                cells.append((ch, fg, bg, bold))
            pos = m.end()
            codes = [int(c) if c else 0 for c in m.group(1).split(';')]
            i = 0
            while i < len(codes):
                c = codes[i]
                if c == 0: fg, bg, bold = 7, 0, False
                elif c == 1: bold = True
                elif c == 22: bold = False
                elif 30 <= c <= 37: fg = c - 30
                elif c == 39: fg = 7
                elif 40 <= c <= 47: bg = c - 40
                elif c == 49: bg = 0
                elif 90 <= c <= 97: fg, bold = c - 90, True
                elif c in (38, 48) and codes[i + 1:i + 2] == [5]:
                    n = codes[i + 2] % 8
                    if c == 38: fg = n
                    else: bg = n
                    i += 2
                i += 1
        for ch in line[pos:]:
            cells.append((ch, fg, bg, bold))
        cells += [(' ', 7, 0, False)] * (COLS - len(cells))
        out, prev = [], None
        for ch, f, b, bo in cells[:COLS]:
            style = (BRIGHT[f] if bo and f else NORMAL[f], NORMAL[b])
            if style != prev:
                if prev: out.append('</span>')
                out.append('<span style="color:%s;background:%s">' % style)
                prev = style
            out.append(html.escape(ch))
        out.append('</span>')
        rows.append(''.join(out))
    rows += [' ' * COLS] * (ROWS - len(rows))
    return '''<!doctype html><html><head><meta charset="utf-8"><style>
body{margin:0;background:#1e1e1e;display:inline-block;padding:18px}
.win{border-radius:8px;overflow:hidden;box-shadow:0 6px 24px rgba(0,0,0,.5);display:inline-block}
.bar{background:#3a3a3a;color:#ddd;font:13px system-ui,sans-serif;padding:7px 12px;text-align:center;position:relative}
.dots{position:absolute;left:12px;top:9px}.dots i{display:inline-block;width:11px;height:11px;border-radius:50%;margin-right:6px}
pre{margin:0;background:#000;font:17px/20px "DejaVu Sans Mono",monospace;padding:6px}
</style></head><body><div class="win"><div class="bar"><span class="dots"><i style="background:#ff5f56"></i><i style="background:#ffbd2e"></i><i style="background:#27c93f"></i></span>@TITLE@</div><pre>@BODY@</pre></div></body></html>'''.replace('@TITLE@', html.escape(title)).replace('@BODY@', '\n'.join(rows))

if __name__ == '__main__':
    src, dst, title = sys.argv[1:4]
    open(dst, 'w').write(render(open(src, encoding='utf-8', errors='replace').read(), title))
