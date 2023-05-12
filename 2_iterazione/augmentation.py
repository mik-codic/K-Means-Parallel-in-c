import csv
import random

def add_noise(data_file, noise_file, n):
    with open(data_file, newline='') as f:
        reader = csv.reader(f)
        header = next(reader)
        data = list(reader)
    
    with open(noise_file, mode='w', newline='') as f:
        writer = csv.writer(f)
        writer.writerow(header)
        data_set = set(tuple(row) for row in data)
        while len(data_set) < n:
            row = random.choice(data)
            new_row = []
            for i, val in enumerate(row[:-1]):
                noise = round(random.uniform(-0.1, 0.1), 1)
                new_val = round(float(val) + noise, 1)
                new_row.append(new_val)
            new_row.append(row[-1])
            new_row = tuple(new_row)
            if new_row not in data_set:
                writer.writerow(new_row)
                data_set.add(new_row)

data_file = 'iris.csv'
noise_file = 'iris_noisy.csv'
n = 200

add_noise(data_file, noise_file, n)
