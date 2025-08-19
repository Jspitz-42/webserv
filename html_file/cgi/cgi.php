<?php

$method = getenv("REQUEST_METHOD") ?: "";
$query  = getenv("QUERY_STRING") ?: "";

header("Content-Type: text/html; charset=utf-8");

?>
<!doctype html>
<html>
  <body>
    <h1>Test CGI PHP</h1>
    <p>Methode HTTP : <?= htmlspecialchars($method) ?></p>
    <p>QUERY_STRING : <?= htmlspecialchars($query) ?></p>
  </body>
</html>
