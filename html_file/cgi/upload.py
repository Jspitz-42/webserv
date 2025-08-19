#!/usr/bin/env python3
import cgi
import os
import sys

form = cgi.FieldStorage()

fileitem = form['file']

if fileitem.filename:
    os.makedirs("html_file/uploads/", exist_ok=True)
    filename = os.path.basename(fileitem.filename)
    filepath = os.path.join("html_file/uploads/", filename)
    with open(filepath, 'wb') as f:
        f.write(fileitem.file.read())