#!/usr/bin/env python
import sys, os
cogFiles = list()
basePath = '.'
if len(sys.argv) > 1:
    basePath = sys.argv[1]
for pathBranch in os.walk(basePath):
   for fileName in pathBranch[2]:
       path = os.path.join(pathBranch[0],fileName)
       if os.system("""bash -c 'nocog {path} &> /dev/null'""".format(**locals())):
            cogFiles.append(path)

for filePath in cogFiles:
    os.system('cog.py -cr ' + filePath)
