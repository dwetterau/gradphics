stlfile = open('./PenroseTriangle.inc','r')
vertices = []
faces = []
for line in stlfile:
  line = line.strip()
  if line[0] == 't':
    continue
  if line[0] == '}':
    faces.append((len(vertices) - 1, len(vertices) - 2, len(vertices) - 3))
    continue
  line = line[1:-1]
  if line[-1] == '>':
    line = line[:-1]
  arr = line.split(', ')
  tup = tuple([float(arr[0]), float(arr[1]), float(arr[2])])
  vertices.append(tup)
print vertices
print faces

