const exampleSocket = new WebSocket("ws://127.0.0.1:8081/websocket");
const myArgs = process.argv.slice(2);
const suggestion_data = {
  room_id: 202,
  suggestion_id: 400,
};
const jsonObject = JSON.stringify({
  action: "ACCEPT_SUGGESTION",
  payload: suggestion_data,
});

exampleSocket.addEventListener("open", (event) => {
  console.log(
    "websocket connection successfully established now sending message",
  );

  exampleSocket.send(jsonObject);
});
exampleSocket.onmessage = (event) => {
  console.log(event.data);
};
