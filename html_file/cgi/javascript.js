#!/usr/bin/node

const method = process.env.REQUEST_METHOD || "";
const query  = process.env.QUERY_STRING || "";

console.log("Content-Type: text/html; charset=utf-8");
console.log("");

console.log(`<!doctype html><html>
<head>
<style> 
#myDIV {
  width: 300px;
  height: 200px;
  border: 1px solid black;
  background-image: url('smiley.gif');
  background-position: top left;
  animation: mymove 5s infinite;
}

@keyframes mymove {
  50% {background-position: center;}
}
</style>
</head>
<body>

<h1>Animation of background-position</h1>

<p>Gradually change the background-position from "top left" to "center", and back to "top left":<p>
<div id="myDIV"></div>

</body>
</html>`);
