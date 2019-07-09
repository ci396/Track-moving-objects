# Databricks notebook source
import numpy as np
import pandas as pd
import seaborn as sns
import matplotlib.pyplot as plt
import math
import os
os.environ["PYSPARK_PYTHON"] = "python3"


# COMMAND ----------

from pyspark.sql import SparkSession
spark = SparkSession \
    .builder \
    .appName("moive analysis") \
    .config("spark.some.config.option", "some-value") \
    .getOrCreate()

# COMMAND ----------

movies = spark.read.load("/FileStore/tables/movies.csv", format='csv', header = True)
ratings = spark.read.load("/FileStore/tables/ratings.csv", format='csv', header = True)
links = spark.read.load("/FileStore/tables/links.csv", format='csv', header = True)
tags = spark.read.load("/FileStore/tables/tags.csv", format='csv', header = True)


# COMMAND ----------

movies.show()

# COMMAND ----------

tmp1 = ratings.groupBy("userID").count().toPandas()['count'].min()
tmp2 = ratings.groupBy("movieId").count().toPandas()['count'].min()
print('For the users that rated movies and the movies that were rated:')
print('Minimum number of ratings per user is {}'.format(tmp1))
print('Minimum number of ratings per movie is {}'.format(tmp2))

# COMMAND ----------

tmp1 = sum(ratings.groupBy("movieId").count().toPandas()['count'] == 1)
tmp2 = ratings.select('movieId').distinct().count()
print('{} out of {} movies are rated by only one user'.format(tmp1, tmp2))

# COMMAND ----------

#Part1 Spark SQL and OLAP


#Q1
Usernum = ratings.select("userID").distinct().count()
print('{} users are shown in this dataset'.format(Usernum))


# COMMAND ----------

#Q2
Movienum = movies.select("movieID").distinct().count()
Movienum1 = movies.select("title").distinct().count()
if Movienum == Movienum1:
  print('No duplicates, {} movies are shown in this dataset'.format(Movienum))
else:
  print('Duplicates!, {} movies are shown in this dataset'.format(Movienum1))
  



# COMMAND ----------

#Find duplicate
from pyspark.sql import functions as func
tmp1 = movies.groupBy("title").agg(func.collect_list("movieId"))
duplicate = tmp1.where(func.size(tmp1["collect_list(movieId)"]) > 1)
display(duplicate)

# COMMAND ----------

#Remove duplicate
movies = movies.dropDuplicates(['title'])
tmp1 = ratings.withColumn('movieId', func.when((func.col('movieId')!=147002), func.col('movieId')).otherwise(32600))
tmp2 = tmp1.withColumn('movieId', func.when((func.col('movieId')!=144606), func.col('movieId')).otherwise(6003))
tmp3 = tmp2.withColumn('movieId', func.when((func.col('movieId')!=26958), func.col('movieId')).otherwise(838))
tmp4 = tmp3.withColumn('movieId', func.when((func.col('movieId')!=168358), func.col('movieId')).otherwise(2851))
ratings = tmp4.withColumn('movieId', func.when((func.col('movieId')!=64997), func.col('movieId')).otherwise(34048))

# COMMAND ----------

#Q3
ratnumber = ratings.select("movieId").distinct().count()
print('{} out of {} movies are rated by users'.format(ratnumber, Movienum1))


# COMMAND ----------

q3_tmp1 = ratings.select("movieId")
q3_tmp2 = movies.select("movieId")
q3 = q3_tmp2.subtract(q3_tmp1).orderBy("movieId")
display(q3)
print('{} out of {} movies are not rated by users'.format(q3.count(), Movienum1))


# COMMAND ----------

#Q4
# genres = movies.toPandas()['genres'].tolist()
genres = movies.select("genres").collect()
genres_list = set()
for g in genres:
  tmp = g[0].split('|')
  for c in tmp:
    genres_list.add(c)
print(genres_list)


# COMMAND ----------

#Q5
category_q5 = movies.withColumn('genres', func.explode(func.split('genres', '\|')))
display(category_q5)

# COMMAND ----------

from pyspark.mllib.recommendation import ALS

movie_rating = sc.textFile("/FileStore/tables/ratings.csv")
header = movie_rating.take(1)[0]
rating_data = movie_rating.filter(lambda line: line!=header).map(lambda line: line.split(",")).map(lambda tokens: (tokens[0],tokens[1],tokens[2])).cache()


# COMMAND ----------

# check three rows
rating_data.take(3)

# COMMAND ----------

train, validation, test = rating_data.randomSplit([6,2,2],seed = 1)
test_for_predict = test.map(lambda x: (x[0], x[1]))


# COMMAND ----------

train.cache()

# COMMAND ----------

validation.cache()

# COMMAND ----------

test.cache()

# COMMAND ----------

def train_ALS(train_data, validation_data, num_iters, reg_param, ranks):
    min_error = float('inf')
    best_rank = -1
    best_regularization = 0
    best_model = None
    validation_for_predict = validation_data.map(lambda x: (x[0], x[1]))
    for rank in ranks:
        for reg in reg_param:
            # write your approach to train ALS model
            # make prediction
            # get the rating result
            # get the RMSE
            model = ALS.train(train_data, rank, seed = 3, iterations = num_iters, lambda_ = reg)
            prediction = model.predictAll(validation_for_predict).map(lambda r: ((r[0], r[1]), r[2]))
            ratesAndPreds = validation_data.map(lambda r: ((int(r[0]), int(r[1])), float(r[2]))).join(prediction)
            error = math.sqrt(ratesAndPreds.map(lambda r: (r[1][0] - r[1][1])**2).mean())
            print ('{} latent factors and regularization = {}: validation RMSE is {}'.format(rank, reg, error))
            if error < min_error:
                min_error = error
                best_rank = rank
                best_regularization = reg
                best_model = model
    print ('\nThe best model has {} latent factors and regularization = {}'.format(best_rank, best_regularization))
    return best_model

# COMMAND ----------

num_iterations = 10
ranks = [6, 8, 10, 12, 14]
reg_params = [0.05, 0.1, 0.2, 0.4, 0.8]

import time
start_time = time.time()
final_model = train_ALS(train, validation, num_iterations, reg_params, ranks)

print ('Total Runtime: {:.2f} seconds'.format(time.time() - start_time))

# COMMAND ----------

iter_array = [1, 2, 5, 10]
def plot_learning_curve(iter_array, train, validation, reg_param, rank):
  validation_for_predict = validation.map(lambda x: (x[0], x[1]))
  error_array = []
  for iter_num in iter_array:
    model = ALS.train(train, rank, seed = 3, iterations = iter_num, lambda_ = reg_param)
    prediction = model.predictAll(validation_for_predict).map(lambda r: ((r[0], r[1]), r[2]))
    ratesAndPreds = validation.map(lambda r: ((int(r[0]), int(r[1])), float(r[2]))).join(prediction)
    error = math.sqrt(ratesAndPreds.map(lambda r: (r[1][0] - r[1][1])**2).mean())
    error_array.append(error)
  fig = plt.figure(figsize=(10,6))
  plt.plot(iter_array, error_array, linestyle = '-',
         linewidth = 2,
         color = 'steelblue',
         marker = 'o',
         markersize = 6,
         markeredgecolor='black',
         markerfacecolor='brown') 
  plt.title('learning curve of the model based on ietration numbers')
  plt.xlabel('iteratio numbers')
  plt.ylabel('error')
  display(fig)
plot_learning_curve(iter_array, train, validation, 0.2, 14)


# COMMAND ----------

test_for_predict = test.map(lambda x: (x[0], x[1]))
prediction = final_model.predictAll(test_for_predict).map(lambda r: ((r[0], r[1]), r[2]))
ratesAndPreds = test.map(lambda r: ((int(r[0]), int(r[1])), float(r[2]))).join(prediction)
error = math.sqrt(ratesAndPreds.map(lambda r: (r[1][0] - r[1][1])**2).mean())
print(error)
