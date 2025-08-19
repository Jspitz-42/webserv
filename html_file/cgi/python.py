#!/usr/bin/python3
import os, sys

print("Content-Type: text/html; charset=utf-8")
print("")  # ligne vide obligatoire

method = os.environ.get("REQUEST_METHOD", "")
query  = os.environ.get("QUERY_STRING", "")

print("<!doctype html><html><body>")
print("<h1>Test CGI Python</h1>")
print("<p>Méthode HTTP : {}</p>".format(method))
print("<p>QUERY_STRING : {}</p>".format(query))
print("</body></html>")
