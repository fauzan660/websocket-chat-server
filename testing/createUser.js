const exampleSocket = new WebSocket("ws://127.0.0.1:8081/websocket");
const myArgs = process.argv.slice(2);
const user_data = {
  user_id: 101,
  username: myArgs[0],
};
const jsonObject = JSON.stringify({
  action: "CREATE_USER",
  payload: user_data,
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
