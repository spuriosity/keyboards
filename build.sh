#!/bin/zsh
# Compile lily58.json with QMK's build API (no local toolchain needed) into build/.
set -euo pipefail
HERE=${0:A:h}
mkdir -p "$HERE/build"
python3 - "$HERE" <<'PY'
import json, sys, time, urllib.request
here = sys.argv[1]
km = json.load(open(f'{here}/lily58.json'))
payload = {k: km[k] for k in ('keyboard', 'keymap', 'layout', 'layers')}
req = urllib.request.Request('https://api.qmk.fm/v1/compile', data=json.dumps(payload).encode(),
                             headers={'Content-Type': 'application/json'})
job = json.load(urllib.request.urlopen(req))['job_id']
for _ in range(100):
    time.sleep(3)
    st = json.load(urllib.request.urlopen(f'https://api.qmk.fm/v1/compile/{job}'))
    if st['status'] in ('finished', 'failed'):
        break
r = st.get('result', {})
if st['status'] != 'finished' or r.get('returncode') != 0:
    sys.exit(f"compile {st['status']}:\n{r.get('output', '')[-3000:]}")
out = f"{here}/build/{r['firmware_filename']}"
open(out, 'wb').write(urllib.request.urlopen(f'https://api.qmk.fm/v1/compile/{job}/download').read())
print('Built', out)
PY
