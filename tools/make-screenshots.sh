#!/bin/sh
# Regenerates docs/screenshots/*.png by driving cfdk inside an 80x24 tmux
# session. Requires tmux, python3, node and Playwright; run as root from the
# top of the source tree after `make`.
set -eu

OUT=docs/screenshots
TMP=$(mktemp -d)
trap 'tmux kill-session -t fdkshot 2>/dev/null || true; kill $FDKD 2>/dev/null || true; rm -rf "$TMP"' EXIT

./fdkd -d -p 7123 >/dev/null 2>&1 &
FDKD=$!
sleep 1

tmux new-session -d -s fdkshot -x 80 -y 24 "TERM=xterm-256color ./cfdk; sleep 60"
sleep 2

key() { tmux send-keys -t fdkshot "$@"; sleep 1.2; }
cap() { tmux capture-pane -t fdkshot -e -N -p >"$TMP/$1.ansi"; }

key F2; key Down Down Down Down Down Down Down Down; cap pci-list
key Enter; key Right Right Right Right Down Space; cap pci-config
key F1; cap help
key F1 F5; cap memory
key Escape

set --
for n in pci-list pci-config help memory; do
  python3 tools/ansi2html.py "$TMP/$n.ansi" "$TMP/$n.html" "cfdk — 80x24"
  set -- "$@" "$TMP/$n.html" "$PWD/$OUT/$n.png"
done
node tools/screenshot.mjs "$@"
