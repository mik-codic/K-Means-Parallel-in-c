
#import libraries 
import numpy as np 
from sklearn.datasets import load_iris 
import matplotlib.pyplot as plt 
import pandas as pd # import pandas library

#load dataset
iris = load_iris() 
X = iris.data # this is a numpy array of shape (150, 4) 
y = iris.target # this is a numpy array of shape (150,) 
feature_names = iris.feature_names # this is a list of strings 
target_names = iris.target_names # this is a list of strings

#define a function to add noise 
def add_noise(data, std): 
    noise = np.random.normal(0, std, data.shape) # this is a numpy array of shape (150, 4) 
    data_noisy = data + noise # this is a numpy array of shape (150, 4) 
    return np.round(data_noisy,1)

#call the add_noise function with different values of std 
X_noisy_01 = add_noise(X, 0.1) # this is a numpy array of shape (150, 4) X_noisy_02 = add_noise(X, 0.2) # this is a numpy array of shape (150, 4) X_noisy_03 = add_noise(X, 0.3) # this is a numpy array of shape (150, 4)

#define a function to save data in csv file 
def save_data(data, file_name): 
    df = pd.DataFrame(data=np.c_[data, y], columns=feature_names + ["target"]) # create dataframe from data and target arrays 
    df["target"] = df["target"].map(dict(zip(np.unique(y), target_names))) # map target values to class names 
    df.to_csv(file_name, sep=",", index=False,quotechar='"') # save dataframe as csv file

#call the save_data function with each noisy data and a file name 
save_data(X_noisy_01, "iris_noisy_01.csv") # save noisy dataset with std=0.1 as csv file 
#save_data(X_noisy_02, ‘iris_noisy_02.csv’) # save noisy dataset with std=0.2 as csv file save_data(X_noisy_03, ‘iris_noisy_03.csv’) # save noisy dataset with std=0.3 as csv file`