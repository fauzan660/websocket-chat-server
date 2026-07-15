# ws-logger

A tiny(0.19mb) binary in pure c++ that sits between your websocket server and client logging data packets passing through
No dependancies needed to run it. Just build and enjoy :)

## build

requires openssl (already on most systems)

```bash
cmake -DCMAKE_BUILD_TYPE=Release -S . -B build
cmake --build build
```

## usage

```bash
./wslog --target ws://localhost:8080 --port 9001
```
- `--target` = host:port where your server is running
- `--port` = port you want the proxy to run on, point your client here instead of where your server is running

## flow
client -> proxy -> server
- no need for client to server direct connection proxy handles that

## output

```
[CLIENT → SERVER] text | {"action":"join_room"}
[SERVER → CLIENT] text | {"status":"ok"}
[CLIENT → SERVER] ping
[SERVER → CLIENT] close
```
