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

def euclidean_distance(l1, l2):
  value = 0 
  for x in range(len(l1)):
    value += pow(l1[x] - l2[x], 2)
  return math.sqrt(value)


input_matrix = import_matrix('input_hw7.txt')
class_labels = import_matrix('class_label_hw7.txt')
query = [4, 3, 2]

results = []

for i in range(len(input_matrix)):
  row = [i]
  row.append(euclidean_distance(input_matrix[i], query))
  row.append(class_labels[i][0]) 
  results.append(row)

results.sort(key = lambda row: row[1])

for i in range(5):
  print('Value ' + str(results[i][1]) + ' Class ' + str(results[i][2]))
