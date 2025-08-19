#!/usr/bin/node

const method = process.env.REQUEST_METHOD || "";
const query  = process.env.QUERY_STRING || "";

console.log("Content-Type: text/html; charset=utf-8");
console.log("");

console.log(`<!doctype html><html><body>
<h1>Test CGI JavaScript</h1>
<p>Méthode HTTP : ${method}</p>
<p>QUERY_STRING : ${query}</p>
</body></html>`);
