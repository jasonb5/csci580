import math

def import_matrix(file):
  f = open(file, 'r')
  matrix = [] 
  for line in f:
    row = []
    for c in line.split(' '):
      if c != '\n': 
        row.append(int(c))
    matrix.append(row)
  return matrix

def dot_prod(l1, l2):
  value = 0
  for i in range(len(l1)):
    value += l1[i] * l2[i]
  return value

def h_func(w, x_j):
  return 1/(1+math.exp(-dot_prod(w, x_j)))

input_matrix = import_matrix('input_hw7.txt')
class_labels = import_matrix('class_label_hw7.txt')
weights = [1, 1, 1, 1]

for row in input_matrix:
  row.insert(0, 1)

for c in range(10):
  for x in range(len(input_matrix)):
    value = h_func(weights, input_matrix[x])
    for y in range(len(weights)):
      weights[y] = weights[y]+(0.01*(class_labels[x][0]-value)*value*(1-value)*input_matrix[x][y])

print(weights)
