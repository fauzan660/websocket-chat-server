const exampleSocket = new WebSocket("ws://127.0.0.1:8081/websocket");
const myArgs = process.argv.slice(2);
const codeObject = {
  code_snippet: myArgs[2],
  error_message: myArgs[3],
  language: myArgs[4],
  context: myArgs[5],
};
const room_data = {
  room_id: 1,
  title: myArgs[0],
  creator_id: parseInt(myArgs[1]),
  code: codeObject,
};
const jsonObject = JSON.stringify({
  action: "CREATE_ROOM",
  payload: room_data,
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
