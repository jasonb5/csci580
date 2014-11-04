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

def calc_euclidian(l1, l2):
  value = 0
  for i in range(len(l1)):
    value += pow(l1[i] - l2[i], 2)
  return math.sqrt(value)

input_matrix = import_matrix('input_hw7.txt')
class_labels = import_matrix('class_label_hw7.txt')
query = [4, 3, 2]

results = {}

for i in range(len(input_matrix)):
  results[i] = calc_euclidian(input_matrix[i], query)

sorted_results = sorted(results.items(), key=lambda x:x[1])

for i in range(5):
  print(str(sorted_results[i][1]) + ' ' + str(class_labels[sorted_results[i][0]][0]))

