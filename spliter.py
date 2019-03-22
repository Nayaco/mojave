#!/usr/bin/python3
import os
import sys
def spliter(input: str, output: str):
    fd0 = open(output, 'w')
    with open(input) as fd1:
        lines = fd1.readlines()
        wordlist = map(lambda x: x.split(','), lines)
        for i in wordlist:
            for word in i:
                fd0.write(word + '\n')
    fd0.close()

spliter(sys.argv[1], sys.argv[2])