#!/usr/bin/env python3
"""Render config/bt60_v2.keymap as text diagrams for LAYOUT.md.

    python3 tools/layout.py > /tmp/layers.md
"""
import re
import sys
from pathlib import Path

KEYMAP = Path(__file__).resolve().parent.parent / 'config' / 'bt60_v2.keymap'
ROWS = [14, 14, 13, 12, 8]
WIDTH = 7

NAMES = {
    'ESC': 'Esc', 'GRAVE': '`', 'MINUS': '-', 'EQUAL': '=', 'BSPC': 'Bspc', 'TAB': 'Tab', 'LBKT': '[', 'RBKT': ']',
    'BSLH': '\\', 'CAPS': 'Caps', 'SEMI': ';', 'SQT': "'", 'RET': 'Enter', 'COMMA': ',', 'DOT': '.', 'FSLH': '/',
    'LSHFT': 'Shift', 'RSHFT': 'Shift', 'LCTRL': 'Ctrl', 'RCTRL': 'Ctrl', 'LALT': 'Alt', 'RALT': 'RAlt',
    'LGUI': 'Cmd', 'RGUI': 'RCmd', 'SPACE': 'Space', 'C_MENU': 'Menu', 'DEL': 'Del', 'INS': 'Ins',
    'PSCRN': 'PrScr', 'SLCK': 'ScrLk', 'PAUSE_BREAK': 'Pause', 'HOME': 'Home', 'END': 'End', 'PG_UP': 'PgUp',
    'PG_DN': 'PgDn', 'UP': '↑', 'DOWN': '↓', 'LEFT': '←', 'RIGHT': '→', 'C_PREV': '⏮', 'C_NEXT': '⏭',
    'C_PP': '⏯', 'C_MUTE': 'Mute', 'C_VOL_UP': 'Vol+', 'C_VOL_DN': 'Vol-', 'C_BRI_UP': 'Bri+', 'C_BRI_DN': 'Bri-',
}
MODS = {'LG': '⌘', 'RG': '⌘', 'LA': '⌥', 'RA': '⌥', 'LC': '⌃', 'RC': '⌃', 'LS': '⇧', 'RS': '⇧'}
SHORT = {'Bspc': 'Bsp', 'Ctrl': 'Ctl', 'Space': 'Spc'}
LAYERS = {'NAV': 'NAV', 'RAISE': 'Fn', 'SYS': 'SYS', '1': 'Fn', '2': 'NAV', '3': 'SYS'}


def key(k):
    m = re.fullmatch(r'(L[GACS]|R[GACS])\((.+)\)', k)
    if m:
        return MODS[m.group(1)] + key(m.group(2))
    if re.fullmatch(r'N\d', k):
        return k[1]
    return NAMES.get(k, k)


def label(b, morphs):
    parts = b[1:].split()
    name, args = parts[0], parts[1:]
    if name == 'trans':
        return '·'
    if name == 'none':
        return '✕'
    if name == 'kp':
        return key(args[0])
    if name in ('mt', 'ctl_bspc'):
        return f'{SHORT.get(key(args[1]), key(args[1]))}/{SHORT.get(key(args[0]), key(args[0]))}'
    if name == 'lt':
        return f'{SHORT.get(key(args[1]), key(args[1]))}/{LAYERS.get(args[0], args[0])}'
    if name == 'mo':
        return LAYERS.get(args[0], args[0])
    if name in morphs:
        return morphs[name]
    return {
        'bt BT_SEL': lambda: f'BT{int(args[1]) + 1}', 'bt BT_NXT': lambda: 'BT▶', 'bt BT_PRV': lambda: 'BT◀',
        'bt BT_CLR': lambda: 'BTclr', 'out OUT_TOG': lambda: 'USB/BT', 'rgb_ug RGB_TOG': lambda: 'Glow',
        'bl BL_TOG': lambda: 'KeyLt', 'bootloader': lambda: 'Boot', 'bat_report': lambda: 'Batt',
        'bat_log': lambda: 'BatLog', 'studio_unlock': lambda: 'Studio', 'sys_reset': lambda: 'Reset',
    }.get(' '.join([name] + args[:1]) if args else name, lambda: b)()


def main():
    s = KEYMAP.read_text()
    # mod-morphs: show the normal key, e.g. "F1"
    morphs = {m.group(1): key(m.group(2)) for m in re.finditer(
        r'(\w+): \w+ \{[^{}]*?behavior-mod-morph[^{}]*?bindings = <&kp (\w+)>', s, re.S)}
    out = []
    for m in re.finditer(r'\n        // ([^\n]*)\n(?:        //[^\n]*\n)*        (\w+) \{\n            bindings = <\n(.*?)\n            >;', s, re.S):
        title, body = m.group(1), m.group(3)
        keys = ['&' + t.strip() for t in body.split('&') if t.strip()]
        if len(keys) != sum(ROWS):
            continue
        labels = [label(k, morphs) for k in keys]
        out.append(f'### {m.group(2)}\n\n{title}\n\n```')
        n = 0
        for r in ROWS:
            out.append(' '.join(f'{l:^{WIDTH}}'[:WIDTH] for l in labels[n:n + r]).rstrip())
            n += r
        out.append('```\n')
    sys.stdout.write('\n'.join(out))


if __name__ == '__main__':
    main()
