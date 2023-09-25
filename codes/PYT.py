# import numpy as np
# import pandas as pd
import matplotlib.pyplot as plt
import numpy as np

# data = pd.read_csv("Augmented_iris.csv")
# data[data.columns[0]]
# print(len(data))
# data.pop(data.columns[0])
# print(data.head)
# data.to_csv("aug_iris_13mil.csv")
import csv

def lbl_to_int():
    #labels_names = ["Iris-setosa","Iris-versicolor","Iris-virginica"]
    labels_names = ["Setosa","Versicolor","Virginica"]
    labels_int = []
    with open("iris_labels_integer_list.csv", 'r') as input_file:
        csv_reader = csv.reader(input_file)
        data = [row[0] for row in csv_reader]
        for i in range(0,len(data)):
            
            if labels_names[0] == data[i]:
                #print(labels_names[0])
                labels_int.append(0)
            if labels_names[1] == data[i]:
                #print(labels_names[1])
                labels_int.append(2)

            if labels_names[2] == data[i]:
                #print(labels_names[2])
                labels_int.append(1)
    with open("iris_labels_integer_list.csv", 'w', newline='') as output_file:
        csv_writer = csv.writer(output_file)
        csv_writer.writerows(enumerate(labels_int))
def remove_first_column(input_csv, output_csv):
    # Read the input CSV file
    with open(input_csv, 'r') as input_file:
        csv_reader = csv.reader(input_file)
        data = [row[1:] for row in csv_reader]  # Exclude the first column

    # Write the modified data to the output CSV file
    with open(output_csv, 'w', newline='') as output_file:
        csv_writer = csv.writer(output_file)
        csv_writer.writerows(data)
def plotting(input_csv,centroids):
    with open(input_csv, 'r') as input_file:
        csv_reader = csv.reader(input_file)
        data = [list(map(float, row[0:4])) for row in csv_reader]  # Convert to float
    with open(centroids, 'r') as input_file2:
        csv_reader_ = csv.reader(input_file2)
        centroids_data = [list(map(float, row[0:4])) for row in csv_reader_]  # Convert to float
        
    data_array = np.array(data)
    plt.figure(figsize=(8, 6))
    
    plt.scatter(data_array[:, 0], data_array[:, 1], c=data_array[:, 2], s=data_array[:, 3]*10, cmap='viridis')
    plt.xlabel(f'Dimension 0')
    plt.ylabel(f'Dimension 3')
    additional_points = np.array([[x, y,z,t] for x, y,z,t in centroids_data])
    plt.scatter(additional_points[:, 0], additional_points[:, 1], color='red', s=100, marker='x', label='Additional Points')
    
    plt.colorbar(label='Color for Dimension 2')
    plt.title('Scatter Plot of 4D Data')
    plt.show()

if __name__ == "__main__":
    input_csv_file = "iris_labels_integer_list.csv"   # Replace with your input CSV file name
    output_csv_file = "iris_labels_integer_list.csv" # Replace with your output CSV file name
    
    #remove_first_column(input_csv_file, output_csv_file)
    print("First column removed and saved to output.csv")
    #lbl_to_int()
    plotting("/Users/edoardomaines/Desktop/UNI/HPC_Project/K-Means-Parallel-in-c/2_iterazione/updated_project/iris_noisy_2.csv","centroids.csv")
    #plotting("iris_noisy_4.csv","centroids.csv")