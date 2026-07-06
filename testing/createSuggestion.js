const exampleSocket = new WebSocket("ws://127.0.0.1:8081/websocket");
const myArgs = process.argv.slice(2);
const suggestion_data = {
  suggestion_id: 1,
  room_id: parseInt(myArgs[0]),
  author_id: parseInt(myArgs[1]),
  content: myArgs[2],
};
const jsonObject = JSON.stringify({
  action: "ADD_SUGGESTION",
  payload: suggestion_data,
});
exampleSocket.addEventListener("open", (event) => {
  exampleSocket.send(jsonObject);
});
