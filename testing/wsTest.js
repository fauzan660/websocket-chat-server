const exampleSocket = new WebSocket("ws://127.0.0.1:8081/websocket");
exampleSocket.addEventListener("open", (event) => {
  exampleSocket.send("foo");
});
