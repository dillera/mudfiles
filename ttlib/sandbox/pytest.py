#!/usr/bin/env python
import sqlite3


FILENAME = 'tintin.db'
CONNECTION = sqlite3.connect(FILENAME)
CURSOR = conn.cursor()


def init(connection=CONNECTION, cursor=CURSOR):
    cur.execute("CREATE TABLE Cars(Id INT, Name TEXT, Price INT)")
