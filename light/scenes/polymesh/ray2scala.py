#! /usr/bin/python2

import sys

filename = sys.argv[1]
outputname = sys.argv[2]

inputFile = open(filename, 'r')
output = open(outputname + '.scala', 'w')

# Header 
print >> output, 'package scalagl'
print >> output, 'object ' + outputname + ' extends ScalaGL {'
print >> output, '  override def drawScene() = {'

# color determining
colorstring = '(1.0f,1.0f,1.0f)'
if len(sys.argv) > 3:
  colorstring = sys.argv[3]
print 'Using colorstring', colorstring
print >> output, '    color (\'color := ' + colorstring + ')'

inPoints = False
inFaces = False
index = 0
linenum = 0
methodLength = 100

for line in inputFile:
  line = line.strip()
  outline = '    '
  if line.startswith('points=('):
    inPoints = True
    continue
  if line.startswith('faces=('):
    inFaces = True
    continue
  if inPoints:
    if line.endswith(');'):
      inPoints = False
    outline += "point ('p" + str(index) + ' := ('
    point = line[1:line.index(')')].split(',')
    for num in point:
      num = num.strip()
      outline += num + 'f, '
    outline = outline[:-2] + '))'
    print >> output, outline
    linenum += 1
    index += 1
  if inFaces:
    if line.endswith(');'):
      inFaces = False
    outline += 'triangle (' 
    face = line[1:line.index(')')].split(',')
    for num in face:
      num = num.strip()
      outline += "'p" + num + ', '
    outline += "'color)"
    print >> output, outline
    linenum += 1
  if linenum > 0 and linenum % methodLength == 0:
    print >> output, '    method' + str(linenum / methodLength)
    print >> output, '  }'
    print >> output, ' '
    print >> output, '  def method' + str(linenum / methodLength) + '() {'

#Footer
print >> output, '    start'
print >> output, '  }'
print >> output, '}'
