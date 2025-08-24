#!/usr/bin/node

const method = process.env.REQUEST_METHOD || "";
const query  = process.env.QUERY_STRING || "";

console.log("Content-Type: text/html; charset=utf-8");
console.log("");

console.log(`<!doctype html><html>
<head>
<style> 
#myDIV {
  height: 800px;
  border: 1px solid black;
  background-image: url('https://media3.giphy.com/media/v1.Y2lkPTc5MGI3NjExZndtNTBrNGloeHFrZmd1NjJxOWVrYWJlemhycWV2dXZwY3MxNTBkMiZlcD12MV9pbnRlcm5hbF9naWZfYnlfaWQmY3Q9Zw/CUMXiHI4b0z8llC7aA/giphy.gif');
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
<div id="myDIV"><span></span></div>

</body>
</html>`);
