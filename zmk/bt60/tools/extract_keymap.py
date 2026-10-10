"""Rebuild ZMK keymap bindings from a CURRENT.UF2 dump (how config/bt60_v2.keymap was first recovered).

    python3 tools/extract_keymap.py dumps/2026-10-10_CURRENT.UF2 [layers.json]

Works on 2022-era ZMK firmware, where each binding is {const char *behavior label, param1, param2} and labels are
strings like "KEY_PRESS". It finds the longest run of such 12-byte entries (the keymap table: 61 keys x layers)
and prints them in .keymap syntax. Key names come from ZMK's headers in ../workspace (run ../setup-workspace.sh).
Newer firmware (ZMK v0.3+) names behaviors differently and may need the BEH set below extended.
"""
import json, re, struct, sys
from pathlib import Path

uf2_path = sys.argv[1]
out = sys.argv[2] if len(sys.argv) > 2 else None
hdr_dir = Path(__file__).resolve().parent.parent.parent / 'workspace/zmk/app/include/dt-bindings/zmk'
ROWS = [14, 14, 13, 12, 8]; NKEYS = sum(ROWS)

# --- UF2 -> flash image ---
d = open(uf2_path, 'rb').read(); blocks = {}
for i in range(0, len(d), 512):
    m0, m1, flags, addr, size = struct.unpack_from('<5I', d, i)
    if m0 == 0x0A324655 and m1 == 0x9E5D5157: blocks[addr] = d[i + 32:i + 32 + size]
BASE = min(blocks); top = max(a + len(b) for a, b in blocks.items())
img = bytearray(b'\xff' * (top - BASE))
for a, b in blocks.items(): img[a - BASE:a - BASE + len(b)] = b
img = bytes(img)

BEH = {'KEY_PRESS', 'TRANS', 'MO', 'LAYER_TAP', 'MOD_TAP', 'BLUETOOTH', 'RGB_UG', 'BCKLGHT', 'BOOTLOAD', 'RESET',
       'NONE', 'TOGGLE_LAYER', 'TO_LAYER', 'KEY_TOGGLE', 'STICKY_KEY', 'STICKY_LAYER', 'GRAVE_ESCAPE', 'EXT_POWER',
       'OUTPUTS', 'CAPS_WORD', 'KEY_REPEAT', 'MACRO'}
def cstr(a):
    o = a - BASE
    if not 0 <= o < len(img): return None
    e = img.find(b'\x00', o, o + 32)
    if e <= o: return None
    t = img[o:e]
    return t.decode() if all(65 <= c <= 90 or 48 <= c <= 57 or c == 95 for c in t) else None
def raw(i):
    p, a, b = struct.unpack_from('<III', img, i)
    s = cstr(p)
    return (s, a, b) if s in BEH else None
best = (0, 0)
for st in range(0, len(img) - 12, 4):
    if not raw(st) or (st >= 12 and raw(st - 12)): continue
    n = 0
    while raw(st + 12 * n): n += 1
    if n > best[1]: best = (st, n)
start, count = best
nlayers = count // NKEYS
print(f'// keymap table at {hex(start + BASE)}: {count} bindings = {nlayers} layers x {NKEYS} keys', file=sys.stderr)
# --- evaluate ZMK's key-name #defines into numeric usages ---
defs = {}
for f in ['hid_usage_pages.h', 'hid_usage.h', 'modifiers.h', 'keys.h']:
    for m in re.finditer(r'^#define\s+(\w+)(\([^)]*\))?\s+(.+?)\s*$', (hdr_dir / f).read_text(), re.M):
        if not m.group(2): defs[m.group(1)] = re.sub(r'//.*|/\*.*?\*/', '', m.group(3)).strip()
env = {'ZMK_HID_USAGE': lambda p, i: (p << 16) | i, 'HID_USAGE': lambda p, i: (p << 16) | i, 'APPLY_MODS': lambda m, k: (m << 24) | k}
def ev(name, depth=0):
    if name in env and not callable(env[name]): return env[name]
    expr = defs[name]
    if depth > 20: raise ValueError
    for tok in set(re.findall(r'\b[A-Za-z_]\w*\b', expr)) - {'HID_USAGE', 'ZMK_HID_USAGE', 'APPLY_MODS'}:
        if tok in defs and tok not in env: env[tok] = ev(tok, depth + 1)
    v = eval(expr, {}, env); env[name] = v; return v
names = {}
preferred = set()
for n in defs:
    try: v = ev(n)
    except Exception: continue
    if isinstance(v, int) and (v >> 16) in (0x07, 0x0C) and (v >> 24) == 0:
        cur = names.get(v)
        if cur is None or (n in preferred and cur not in preferred) or (not (cur in preferred) and len(n) < len(cur)): names[v] = n
names.update({0x7002f: 'LBKT', 0x70030: 'RBKT', 0x70035: 'GRAVE', 0x7002c: 'SPACE'})
MODS = [(0x01, 'LC'), (0x02, 'LS'), (0x04, 'LA'), (0x08, 'LG'), (0x10, 'RC'), (0x20, 'RS'), (0x40, 'RA'), (0x80, 'RG')]
def key(v):
    s = names.get(v & 0xFFFFFF, hex(v & 0xFFFFFF))
    for bit, f in reversed(MODS):
        if (v >> 24) & bit: s = f'{f}({s})'
    return s
BT = {0: 'BT_CLR', 1: 'BT_NXT', 2: 'BT_PRV'}; BL = {0: 'BL_ON', 1: 'BL_OFF', 2: 'BL_TOG', 3: 'BL_INC', 4: 'BL_DEC', 5: 'BL_CYCLE'}
def binding(i):
    beh, a, b = raw(start + 12 * i)
    return {'KEY_PRESS': lambda: f'&kp {key(a)}', 'TRANS': lambda: '&trans', 'NONE': lambda: '&none',
            'MO': lambda: f'&mo {a}', 'LAYER_TAP': lambda: f'&lt {a} {key(b)}', 'MOD_TAP': lambda: f'&mt {key(a)} {key(b)}',
            'TOGGLE_LAYER': lambda: f'&tog {a}', 'TO_LAYER': lambda: f'&to {a}',
            'BLUETOOTH': lambda: f'&bt {BT.get(a, a)}' + (f' {b}' if a == 3 else ''), 'RGB_UG': lambda: '&rgb_ug RGB_TOG' if a == 0 else f'&rgb_ug {a} {b}',
            'BCKLGHT': lambda: f'&bl {BL.get(a, a)}', 'BOOTLOAD': lambda: '&bootloader', 'RESET': lambda: '&sys_reset'}.get(beh, lambda: f'/* {beh} {a:#x} {b:#x} */')()
layers = [[binding(l * NKEYS + k) for k in range(NKEYS)] for l in range(nlayers)]
if out: json.dump(layers, open(out, 'w'), indent=1)
for l, L in enumerate(layers):
    print(f'--- layer {l}'); i = 0
    for r in ROWS: print('   ', '  '.join(L[i:i + r])); i += r
