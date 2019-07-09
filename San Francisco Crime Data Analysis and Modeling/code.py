# Databricks notebook source
from csv import reader
from pyspark.sql import Row
from pyspark.sql import SparkSession
from pyspark.sql.types import *
from pyspark.sql.functions import *
import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
import warnings
from pyspark.sql.window import Window 
import os
os.environ["PYSPARK_PYTHON"] = "python3"

# COMMAND ----------

import urllib.request

urllib.request.urlretrieve("https://data.sfgov.org/api/views/tmnf-yvry/rows.csv?accessType=DOWNLOAD", "/tmp/sf_data_2003_2018.csv")
dbutils.fs.mv("file:/tmp/sf_data_2003_2018.csv", "dbfs:/FileStore/tables/sf_data_2003_2018.csv")

# COMMAND ----------

# Data Preprocessing
### Input csv as RDD and registered RDD as Dataframe
crime_data_lines = sc.textFile('/FileStore/tables/sf_data_2003_2018.csv')

df_crimes = crime_data_lines.map(lambda line: [x.strip('"') for x in next(reader([line]))])
header = df_crimes.first()
print(header)

crimes = df_crimes.filter(lambda x: x != header)
print(crimes.count())


# COMMAND ----------


def createRow(keys, values):
  assert len(keys) == len(values)
  mapped = dict(zip(keys, values))
  return Row(**mapped)

rdd_rows = crimes.map(lambda x: createRow(header, x))

df_opt2 = spark.createDataFrame(rdd_rows)
df_opt2.createOrReplaceTempView("sf_crime")

# Drop useless columns
df_opt2 = df_opt2.select('IncidntNum', 'Category', 'Descript', 'DayOfWeek', 'Date', 'Time', 'PdDistrict', 'Resolution', 'Address', 'X', 'Y', 'Location', 'PdId')


# Drop duplicates
df_opt2.dropDuplicates()

# Check missing values
df_opt2.select([count(when(isnan(c) | col(c).isNull() | (col(c) == ''), c)).alias(c) for c in df_opt2.columns]).show()



# COMMAND ----------

# Drop Missing Values
df_opt2 = df_opt2.replace('', None)
df_opt2 = df_opt2.na.drop()
df_opt2.select([count(when(isnan(c) | col(c).isNull() | (col(c) == ''), c)).alias(c) for c in df_opt2.columns]).show()

# COMMAND ----------

df_opt2.cache()
display(df_opt2)

# COMMAND ----------
### OLAP
### Write a Spark program that counts the number of crimes for different category.
import matplotlib as mp
number_crime_category = df_opt2.groupBy('Category').count().orderBy('count', ascending = False)
#visualization in Spark
# display(number_crime_category)

# #visualization in Pandas
crimes_pd_df = number_crime_category.toPandas()
# crimes_pd_df["count"] = crimes_pd_df["count"].astype("int")

#set up figure
figQ1, ax = plt.subplots(figsize = (15, 5))

# set poisition of each bar
loc = np.arange(len(crimes_pd_df['Category']))
loc_strech = [2.0 * i for i in loc]

barQ1 = ax.bar(loc_strech, crimes_pd_df['count'], width = 1.5, align = 'center')

#set image coordinates border
ax.spines['top'].set_visible(False)
ax.spines['left'].set_visible(False)
ax.spines['bottom'].set_visible(False)

#set x-axis format
ax.set_xticks(loc_strech)
ax.set_xticklabels(labels = crimes_pd_df['Category'], fontsize = '6', rotation = -45, verticalalignment = 'top', horizontalalignment = 'left')

#set y-axis format
ax.get_yaxis().set_major_formatter(
  mp.ticker.FuncFormatter(lambda x, p : str(x)[:-5]+' K' if x!=0 else '0'))
ax.title.set_text('Number of Crime in SF by Category')

#function to show value of each bar
def showlabel(bars):
    for bar in bars:
        h = bar.get_height()
#         print(type(bar.get_height()))
#         print(type(bar.get_x()))
#         print(type(bar.get_width()))
        ax.text(bar.get_x() + bar.get_width()*0.0, 1.02*h, '{}'.format(int(h)), ha = 'left', va = 'bottom', rotation = 45)

showlabel(barQ1)
plt.tight_layout()
display(figQ1)

# COMMAND ----------
### OLAP
### Counts the number of crimes for different district, and visualize your results
number_crime_district = df_opt2.groupBy('PdDistrict').count().orderBy('count', ascending = False)

crime_district_pd_df = number_crime_district.toPandas()

figQ2, ax = plt.subplots(figsize = (10, 5))
 
loc = np.arange(len(crime_district_pd_df['PdDistrict']))
loc_strech = [2.5 * i for i in loc]

barQ2 = ax.bar(loc_strech, crime_district_pd_df['count'], width = 1.5, align = 'center', color = 'green')

ax.spines['top'].set_visible(False)
ax.spines['left'].set_visible(False)
ax.spines['bottom'].set_visible(False)
ax.set_xticks(loc_strech)
ax.set_xticklabels(labels = crime_district_pd_df['PdDistrict'], fontsize = '7', rotation = -45, verticalalignment = 'top', horizontalalignment = 'left')

ax.get_yaxis().set_major_formatter(
  mp.ticker.FuncFormatter(lambda x, p : str(x)[:-5]+' K' if x!=0 else '0'))
ax.title.set_text('Number of Crime in SF by District')

def showlabel(bars):
    for bar in bars:
        h = bar.get_height()
#         print(type(bar.get_height()))
#         print(type(bar.get_x()))
#         print(type(bar.get_width()))
        ax.text(bar.get_x() + bar.get_width()*0.0, 1.01*h, '{}'.format(int(h)), ha = 'left', va = 'bottom', rotation = 45, fontsize = '7')

showlabel(barQ2)
plt.tight_layout()
display(figQ2)

# COMMAND ----------

#Q3
# filter sunday data
### OLAP
### Count the number of crimes each "Sunday" at "SF downtown".
df_pos = df_opt2.filter(df_opt2['DayOfWeek'] == 'Sunday')


# convert latitude and longitude
df_pos = df_pos.withColumn('Latitude', df_pos['Y'].cast('float'))
df_pos = df_pos.withColumn('Longitude', df_pos['X'].cast('float'))


# choose SF downtown
df_pos = df_pos.filter((df_pos['Latitude'] >= 37.773351) & (df_pos['Latitude'] <= 37.782669) & (df_pos['Longitude'] >= -122.422945) & (df_pos['Longitude'] <= -122.412498))

# Groupby date
df_q3res = df_pos.groupBy('Date').count().orderBy('count', ascending = False)
display(df_q3res)

# convert to pandas dataframe to plot scatter
df_posp = df_pos.toPandas()




# plot scatter figure
df_posp.plot.scatter(x = 'Latitude', y = 'Longitude')
display()


# COMMAND ----------

### OLAP
### Analysis the number of crime in each month of 2015, 2016, 2017, 2018. Then, give insights for the output results and business impact for my result
# method 1

df_q4 = df_opt2.select('Date', to_timestamp('Date', 'MM/dd/yyyy').alias('Date1'))


df_q4 = df_q4.filter((year(df_q4['Date1']) == '2015') | (year(df_q4['Date1']) == '2016') | (year(df_q4['Date1']) == '2017'))
df_q41 = df_q4.groupBy(month('Date1').alias('month')).count().orderBy('count', ascending = False)
# display(df_q4_res)

# method 2
# df_q4 = df_opt2.toPandas()
# df_q4['Date'] = pd.to_datetime(df_q4['Date'])
# df_q4_res = df_q4.groupby(df_q4['Date'].dt.strftime('%B')).size().reset_index(name='counts').sort_values('counts', ascending = False)
# display(df_q4_res)

df_q4_res = df_q41.toPandas()
figQ4, ax = plt.subplots(figsize = (10, 5))
 
loc = np.arange(len(df_q4_res['month']))
loc_strech = [1.5 * i for i in loc]

barQ4 = ax.bar(loc_strech, df_q4_res['count'], width = 1, align = 'center', color = 'sienna')

ax.spines['top'].set_visible(False)
ax.spines['left'].set_visible(False)
ax.spines['bottom'].set_visible(False)
ax.set_xticks(loc_strech)
ax.set_xticklabels(labels = df_q4_res['month'], fontsize = '7', rotation = -45, verticalalignment = 'top', horizontalalignment = 'left')

ax.get_yaxis().set_major_formatter(
  mp.ticker.FuncFormatter(lambda x, p : str(x)[:-5]+' K' if x!=0 else '0'))
ax.title.set_text('Number of Crime in SF by Month')

def showlabel(bars):
    for bar in bars:
        h = bar.get_height()
#         print(type(bar.get_height()))
#         print(type(bar.get_x()))
#         print(type(bar.get_width()))
        ax.text(bar.get_x() + bar.get_width()*0.0, 1.01*h, '{}'.format(int(h)), ha = 'left', va = 'bottom', rotation = 45, fontsize = '7')

showlabel(barQ4)
plt.tight_layout()
display(figQ4)



# COMMAND ----------

### OLAP
### Analysis the number of crime w.r.t the hour in certian day like 2015/12/15, 2016/12/15, 2017/12/15. Then, give travel suggestion to visit SF.
# df_q5 = df_opt2.withColumn('Incident Date', df_opt2['Incident Datetime'].cast('date'))
# df_q5 = df_opt2.groupBy('Incident Date').count().orderBy('Incident Date', ascending = True)
# display(df_q5)

df_q5 = df_opt2.select(to_timestamp('Date', 'MM/dd/yyyy').alias('Date1'), hour(to_timestamp('Time', 'HH:mm')).alias('Hour'))

# Use orderBy
df_q5res = df_q5.groupBy('Date1', 'Hour').count().orderBy(['Date1', 'Hour'], ascending = [1, 1])
# Use sort
# df_q5res = df_q5.groupBy('Date', 'Hour').count().sort(asc('Date'), asc('Hour')) 

# Choose a certain day
d = '2017-03-06'
df_q5res1 = df_q5res.filter(df_q5res['Date1'] == d)

df_q5_pd = df_q5res1.toPandas()

plt.style.use('ggplot')
figQ5 = plt.figure(figsize=(10,6))
plt.plot(df_q5_pd['Hour'], df_q5_pd['count'], linestyle = '-',
         linewidth = 2,
         color = 'steelblue',
         marker = 'o',
         markersize = 6,
         markeredgecolor='black',
         markerfacecolor='brown')


plt.title('Crime Distribution of {}'.format(d))
plt.xlabel('Hour')
plt.ylabel('Count')


ax = plt.gca()

plt.tick_params(top = 'off', right = 'off')


xlocator = mp.ticker.MultipleLocator(1)
ax.xaxis.set_major_locator(xlocator)

figQ5.autofmt_xdate(rotation = -45)

for a,b in zip(df_q5_pd['Hour'],df_q5_pd['count']):
    ax.text(a, 1.01*b, '{}'.format(int(b)), ha='center', va= 'bottom',fontsize=9)


display(figQ5)



# COMMAND ----------

### OLAP
### (1) Step1: Find out the top-3 danger disrict
###(2) Step2: find out the crime event w.r.t category and time (hour) from the result of step 1
### (3) give advice to distribute the police based on your analysis results.
q6_df = df_opt2.select('Time', to_timestamp('Time', 'HH:mm').alias('NewTime'), 'PdDistrict', 'Category', 'Descript').where((df_opt2['PdDistrict'] == 'SOUTHERN') | (df_opt2['PdDistrict'] == 'MISSION') | (df_opt2['PdDistrict'] == 'NORTHERN'))
q6_df_southern = df_opt2.select('Time', to_timestamp('Time', 'HH:mm').alias('NewTime'), 'PdDistrict', 'Category', 'Descript').where(df_opt2['PdDistrict'] == 'SOUTHERN')
q6_df_mission = df_opt2.select('Time', to_timestamp('Time', 'HH:mm').alias('NewTime'), 'PdDistrict', 'Category', 'Descript').where(df_opt2['PdDistrict'] == 'MISSION')
q6_df_northern = df_opt2.select('Time', to_timestamp('Time', 'HH:mm').alias('NewTime'), 'PdDistrict', 'Category', 'Descript').where(df_opt2['PdDistrict'] == 'NORTHERN')

# COMMAND ----------

q6_df_southern_category = q6_df_southern.select('Category', 'Descript').groupBy('Category').count().orderBy('count', ascending=False)
q6_df_southern_category.head(3)

# COMMAND ----------

q6_df_mission_category = q6_df_mission.select('Category').groupBy('Category').count().orderBy('count', ascending=False)
q6_df_mission_category.head(3)

# COMMAND ----------

mission_otheroffenses = df_opt2.where((df_opt2['Category']=='OTHER OFFENSES') & (df_opt2['PdDistrict'] == 'MISSION'))\
                           .groupBy('Descript').count().orderBy('count', ascending=False)
display(mission_otheroffenses)

# COMMAND ----------

q6_df_northern_category = q6_df_northern.select('Category').groupBy('Category').count().orderBy('count', ascending=False)
q6_df_northern_category.head(3)
plt.cla()
# visualization of SOUTHERN district
crimes_pd_df_q6_southern_category = q6_df_southern_category.toPandas()
fig1, ax1 = plt.subplots(figsize=(10,5))
x1 = np.arange(1, q6_df_southern_category.count()+1, 1)
y1 = crimes_pd_df_q6_southern_category['count']
bar_width = 0.6
plt.bar(x1,y1,bar_width)
ax1.set_ylabel('Count')
ax1.set_xlabel('Category')
ax1.set_xticks(x1)
ax1.set_xticklabels(crimes_pd_df_q6_southern_category['Category'], rotation='-45', fontsize='7', horizontalalignment='left')
ax1.spines['right'].set_visible(False)
ax1.spines['top'].set_visible(False)
plt.tight_layout()
display(plt.show())


# COMMAND ----------

plt.cla()
# visualization of MISSION district
crimes_pd_df_q6_mission_category = q6_df_mission_category.toPandas()
fig2, ax2 = plt.subplots(figsize=(10,5))
x2 = np.arange(1, q6_df_mission_category.count()+1, 1)
y2 = crimes_pd_df_q6_mission_category['count']
bar_width = 0.6
plt.bar(x2,y2,bar_width)
ax2.set_ylabel('Count')
ax2.set_xlabel('Category')
ax2.set_xticks(x2)
ax2.set_xticklabels(crimes_pd_df_q6_mission_category['Category'], rotation='-45', fontsize='7', horizontalalignment='left')
ax2.spines['right'].set_visible(False)
ax2.spines['top'].set_visible(False)
plt.tight_layout()
display(plt.show())

# COMMAND ----------

plt.cla()
# visualization of NORTHERN district
crimes_pd_df_q6_northern_category = q6_df_northern_category.toPandas()
fig3, ax3 = plt.subplots(figsize=(10,5))
x3 = np.arange(1, q6_df_northern_category.count()+1, 1)
y3 = crimes_pd_df_q6_northern_category['count']
bar_width = 0.6
plt.bar(x3,y3,bar_width)
ax3.set_ylabel('Count')
ax3.set_xlabel('Category')
ax3.set_xticks(x3)
ax3.set_xticklabels(crimes_pd_df_q6_northern_category['Category'], rotation='-45', fontsize='7', horizontalalignment='left')
ax3.spines['right'].set_visible(False)
ax3.spines['top'].set_visible(False)
plt.tight_layout()
display(plt.show())

# COMMAND ----------

q6_df_time = q6_df.select('NewTime').groupBy(hour('NewTime').alias('time')).count().orderBy('count', ascending=False)
display(q6_df_time)


# COMMAND ----------

plt.cla()
# visualization
crimes_pd_df_q6_time = q6_df_time.toPandas()
fig, ax = plt.subplots(figsize=(10,5))
x = np.arange(1, q6_df_time.count()+1, 1)
y = crimes_pd_df_q6_time['count']
bar_width = 0.6
plt.bar(x,y,bar_width)
ax.set_ylabel('Count')
ax.set_xlabel('time')
ax.set_xticks(x)
ax.set_xticklabels(crimes_pd_df_q6_time['time'], fontsize='7')
ax.spines['right'].set_visible(False)
ax.spines['top'].set_visible(False)
plt.tight_layout()
display(plt.show())


# COMMAND ----------

### OLAP
### For different category of crime, find the percentage of resolution. Based on the output, give hints to adjust the policy.
from pandas import DataFrame
df_q7 = df_opt2.groupBy('Resolution').count().orderBy('count', ascending = False)
df_q7 = df_q7.withColumn('Percentage', df_q7['count'] / df_opt2.count())

df_q7_other = df_q7.filter(df_q7['Percentage'] < 0.01)
df_q7_main = df_q7.filter(df_q7['Percentage'] > 0.01)
df_q7_tmp = df_q7_other.select('count', 'Percentage')

df_q7_main_pd = df_q7_main.toPandas()
df_q7_tmp_pd = df_q7_tmp.toPandas()
display(df_q7_main_pd)
df_q7_tmp_pd.loc['12'] = df_q7_tmp_pd.apply(lambda x: x.sum())
df_q7_main_pd.loc['3'] = df_q7_tmp_pd.loc['12']
df_q7_main_pd.loc['3', 'Resolution'] = 'OTHERS'
# # df_q7_main_pd.info()
# # display(df_q7_main_pd)
# # plt.figure()
# # # size = df_q7_main_pd.select('Percentage')
# # # size.show()
# # label = df_q7_main_pd['Resolution']
# # figQ7 = df_q7_main_pd.plot.pie(size, labels = 'Resolution')
# # display(plt.show())
labels = df_q7_main_pd['Resolution']
sizes = df_q7_main_pd['Percentage']
fig1, ax1 = plt.subplots()
ax1.pie(sizes, labels=labels, autopct='%.2f', shadow=True, startangle=30)
ax1.axis('equal')  # Equal aspect ratio ensures that pie is drawn as a circle.
display(plt.show())


# COMMAND ----------

### Use Kmeans models
from pyspark.ml.feature import VectorAssembler
from pyspark.ml.clustering import KMeans
df_q8 = df_opt2.withColumn('Y', df_opt2['Y'].cast(DoubleType()))
df_q81 = df_q8.withColumn('X', df_q8['X'].cast(DoubleType()))

vecAssembler = VectorAssembler(inputCols=["Y", "X"], outputCol="features")
q8 = vecAssembler.transform(df_q81)
display(q8)





# COMMAND ----------

error = []
for i in range(2,10):
  kmeans = KMeans(k=i, seed=30)  # i clusters here
  model = kmeans.fit(q8.select('features'))
  wssse = model.computeCost(q8)
  error.append(wssse)
print (error)

# COMMAND ----------

x = np.arange(2,10,1)
fig, ax = plt.subplots()
ax.plot(x, error)
display(fig)


# COMMAND ----------

kmeans = KMeans(k=4, seed=30)  # 5 clusters here
model = kmeans.fit(q8.select('features'))
transformed = model.transform(q8)
display(transformed)

# COMMAND ----------

# Evaluate clustering by computing Within Set Sum of Squared Errors.
wssse = model.computeCost(q8)
print("Within Set Sum of Squared Errors = " + str(wssse))

# Shows the result.
centers = model.clusterCenters()
print("Cluster Centers: ")
for center in centers:
    print(center)
