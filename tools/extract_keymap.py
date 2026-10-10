"""Locate the QMK keymaps[][MATRIX_ROWS][MATRIX_COLS] array in a raw flash dump and emit a Configurator keymap.json.

    python3 tools/extract_keymap.py dump.bin keycodes.json info.json out.json

keycodes.json: https://keyboards.qmk.fm/v1/constants/keycodes_0.0.9.json (QMK keycodes, 2022+ numbering)
info.json:     https://keyboards.qmk.fm/v1/keyboards/splitkb/aurora/lily58/rev1/info.json
Finds the layer array by its shape: unused matrix positions are KC_NO and every word decodes as a keycode.
Verified on 2026-10-09 against a compiled .hex (converted to binary): every key matched lily58.json.
"""
import json, struct, sys
dump, kcfile, infofile, outfile = sys.argv[1:5]
data = open(dump, 'rb').read()
kc = {int(k, 16): v for k, v in json.load(open(kcfile))['keycodes'].items()}
info = list(json.load(open(infofile))['keyboards'].values())[0]
ROWS, COLS = info['matrix_size']['rows'], info['matrix_size']['cols']
layout = [tuple(k['matrix']) for k in info['layouts']['LAYOUT']['layout']]
used = set(layout)
unused = [(r, c) for r in range(ROWS) for c in range(COLS) if (r, c) not in used]
N = ROWS * COLS
MODS = ['CTL', 'SFT', 'ALT', 'GUI']

def modname(m):
    side = 'R' if m & 0x10 else 'L'
    return [side + MODS[i] for i in range(4) if m & (1 << i)]

def basic(code):
    v = kc.get(code)
    return v['key'] if v else None

def name(code):
    if code in kc:
        return kc[code]['key']
    lo, hi = code & 0xFF, code >> 8
    if 0x0100 <= code <= 0x1FFF:              # mods + key, e.g. LSFT(KC_1)
        b = basic(lo); ms = modname(hi & 0x1F)
        if b is None or not ms: return None
        s = b
        for m in reversed(ms): s = f'{m[0]}{m[1:] if m[1:]!="CTL" else "CTL"}({s})'.replace('LCTL', 'LCTL')
        return s
    if 0x2000 <= code <= 0x3FFF:              # mod-tap
        b = basic(lo); ms = modname(hi & 0x1F)
        if b is None or not ms: return None
        return f'{ms[0]}_T({b})' if len(ms) == 1 else f'MT({"|".join("MOD_" + m for m in ms)},{b})'
    if 0x4000 <= code <= 0x4FFF:              # layer-tap
        b = basic(lo)
        return f'LT({(code >> 8) & 0xF},{b})' if b else None
    if 0x5000 <= code <= 0x51FF:              # layer-mod
        return f'LM({(code >> 5) & 0xF},{"|".join("MOD_" + m for m in modname(code & 0x1F))})'
    for base, fn in [(0x5200, 'TO'), (0x5220, 'MO'), (0x5240, 'DF'), (0x5260, 'TG'), (0x5280, 'OSL'), (0x52C0, 'TT'), (0x52E0, 'PDF')]:
        if base <= code < base + 0x20: return f'{fn}({code - base})'
    if 0x52A0 <= code < 0x52C0:
        return f'OSM({"|".join("MOD_" + m for m in modname(code & 0x1F))})'
    if 0x5700 <= code <= 0x57FF: return f'TD({code & 0xFF})'
    if 0x7700 <= code <= 0x777F: return f'QK_MACRO_{code & 0x7F}'
    if 0x7E00 <= code <= 0x7E3F: return f'QK_KB_{code & 0x3F}'
    if 0x7E40 <= code <= 0x7FFF: return f'QK_USER_{code - 0x7E40}'
    return None

def words(off):
    if off + 2 * N > len(data): return None
    return struct.unpack_from(f'<{N}H', data, off)

def is_layer(off, first):
    w = words(off)
    if not w: return False
    if any(w[r * COLS + c] != 0 for r, c in unused): return False
    if any(name(x) is None for x in w): return False
    if first:  # base layer: mostly plain basic keys, letters present
        letters = sum(0x04 <= x <= 0x1D for x in w)
        return letters >= 15
    return True

cands = [o for o in range(len(data) - 2 * N) if is_layer(o, True)]
print('base-layer candidates at:', [hex(o) for o in cands])
if not cands: sys.exit('no keymap found')
start = cands[0]; layers = []
off = start
while is_layer(off, False) and len(layers) < 32:
    layers.append(words(off)); off += 2 * N
# trailing all-zero "layers" are not real
while layers and all(x == 0 for x in layers[-1]): layers.pop()
print(f'{len(layers)} layers starting at {hex(start)}')
out = []
for li, w in enumerate(layers):
    keys = [name(w[r * COLS + c]) for r, c in layout]
    out.append(keys)
    print(f'--- layer {li} ---')
    print(' '.join(keys))
json.dump({'version': 1, 'notes': f'Extracted from flash dump at offset {hex(start)}', 'documentation': '',
           'keyboard': 'splitkb/aurora/lily58/rev1', 'keymap': 'extracted_from_board', 'layout': 'LAYOUT',
           'layers': out, 'author': ''}, open(outfile, 'w'), indent=1)
print('wrote', outfile)
