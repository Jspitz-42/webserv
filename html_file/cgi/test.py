#!/usr/bin/env python3 #chatGPT
import os
import cgitb

cgitb.enable(display=0, logdir="/logDir")

print("Content-Type: text/html")
print()

print("<html><body>")
print("<h1>Test CGI Python</h1>")
print("Bonjour le monde")

method = os.environ.get("REQUEST_METHOD", "")
print(f"<p>Méthode HTTP : {method}</p>")

print("</body></html>")