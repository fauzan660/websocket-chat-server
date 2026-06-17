const exampleSocket = new WebSocket("ws://127.0.0.1:8081/websocket");

exampleSocket.addEventListener("open", (event) => {
  console.log(
    "websocket connection successfully established now sending message",
  );

  exampleSocket.send("foo");
});
exampleSocket.onmessage = (event) => {
  console.log(event.data);
};
