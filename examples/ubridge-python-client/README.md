### Python (pynng)

Example project: `examples/ubridge-python-client/`

Usage:

```bash
cd examples/ubridge-python-client
python3 -m venv .venv && source .venv/bin/activate
pip install -r requirements.txt

# REQ/REP examples
python ubridge_client.py

# Subscribe to all sensor data
python ubridge_client.py stream
```

If you see `pynng.exceptions.PermissionDenied` when using IPC, either run with `sudo`, adjust the service to create world/group-readable sockets (see server Troubleshooting), or switch both server and client to TCP.

Run with sudo using the venv interpreter explicitly:

```bash
# From examples/ubridge-python-client with the venv activated/created
sudo -E "$(pwd)/.venv/bin/python" ubridge_client.py
sudo -E "$(pwd)/.venv/bin/python" ubridge_client.py stream
```
