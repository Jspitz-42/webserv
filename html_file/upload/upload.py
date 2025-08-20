#!/usr/bin/env python3
import cgi
import os
import sys

form = cgi.FieldStorage()

fileitem = form['file']

if fileitem.filename:
    os.makedirs("../uploads/", exist_ok=True)
    filename = os.path.basename(fileitem.filename)
    filepath = os.path.join("../uploads/", filename)
    with open(filepath, 'wb') as f:
        f.write(fileitem.file.read())