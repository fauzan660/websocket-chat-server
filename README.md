# wslog

A tiny(0.19mb) binary that sits between your websocket server and client logging data packets passing through

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

point your client at `9001` instead of `8080`, one line change, client and server won't know it's there

## output

```
[CLIENT → SERVER] text | {"action":"join_room"}
[SERVER → CLIENT] text | {"status":"ok"}
[CLIENT → SERVER] ping
[SERVER → CLIENT] close
```
