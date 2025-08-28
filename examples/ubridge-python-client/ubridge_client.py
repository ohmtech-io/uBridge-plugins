#!/usr/bin/env python3
import json
import sys
import time
from typing import Optional

try:
    import pynng
except ImportError:
    print("Missing dependency: pynng. Install with: pip install pynng", file=sys.stderr)
    sys.exit(1)

CONFIG_URL = 'ipc:///tmp/ubridgeReqResp'
STREAM_URL = 'ipc:///tmp/ubridgeStream'


def req(request: dict, timeout_ms: int = 3000) -> Optional[dict]:
    with pynng.Req0(dial=CONFIG_URL, recv_timeout=timeout_ms) as sock:
        payload = json.dumps(request).encode('utf-8')
        sock.send(payload)
        try:
            resp = sock.recv()
        except pynng.Timeout:
            return None
        return json.loads(resp.decode('utf-8'))


def run_examples() -> None:
    devices = req({"request": "getDevices"})
    print("Devices:", json.dumps(devices, indent=2))

    stats = req({"request": "getStatistics"})
    print("Statistics:", json.dumps(stats, indent=2))


def stream_all() -> None:
    print(f"Subscribing to {STREAM_URL} with prefix /sensors")
    with pynng.Sub0(dial=STREAM_URL) as sub:
        sub.subscribe(b"/sensors")
        while True:
            data = sub.recv().decode('utf-8', errors='replace')
            sep = data.find('#')
            if sep <= 0:
                continue
            topic = data[:sep]
            payload = data[sep + 1:]
            try:
                j = json.loads(payload)
            except json.JSONDecodeError:
                continue
            print(topic, json.dumps(j, separators=(',', ':'), ensure_ascii=False))


if __name__ == '__main__':
    if len(sys.argv) > 1 and sys.argv[1] == 'stream':
        stream_all()
    else:
        run_examples()
