import serial
import pymongo
import time
from pymongo import MongoClient

# Connect to MongoDB
client = MongoClient('localhost', 27017)
db = client["magicClock"]
collection = db["Locations1"]

# Initialize serial communication
ser = serial.Serial('COM5', 9600)

dataTable = {
    'Name': 'Skyler',
    'Latitude': '0',
    'Longitude': '0'
}

previous_location = None


def locationName(latitude, longitude):
    lat = float(latitude)
    lon = float(longitude)
    if (36.1908 <= lat <= 36.1913) and (94.5631 <= lon <= 94.5622):
        return "Balzer Technology"
    elif (36.1905 <= lat <= 36.1911) and (94.5610 <= lon <= 94.5605):
        return "Bell Science"
    elif (36.1905 <= lat <= 36.1906) and (94.5597 <= lon <= 94.5594):
        return "Angel Statue"
    elif (36.187 <= lat <= 36.188) and (94.557 <= lon <= 94.558):
        return "Chapel"
    elif (36.185740 <= lat <= 36.18589) and (94.540227 <= lon <= 94.54052):
        return "Work"
    elif (36.18589 <= lat <= 36.185740) and (94.5047 <= lon <= 94.54052):
        return "Home"
    elif (36.307229 <= lat <= 36.30764) and (94.159980 <= lon <= 94.160360):
        return "Parents'"
    else:
        return "In Transit"


def getLocation():
    global dataTable
    data_from_mongo = collection.find_one({'Name': 'Skyler'})
    if data_from_mongo:
        dataTable['Latitude'] = data_from_mongo.get('Latitude', '')
        dataTable['Longitude'] = data_from_mongo.get('Longitude', '')


def sendLocation(location):
    ser.write(location.encode())


while True:
    getLocation()
    print("Latitude:", dataTable['Latitude'], "Longitude:", dataTable['Longitude'])
    current_location = locationName(dataTable["Latitude"], dataTable["Longitude"])
    print("Current location:", current_location)

    # Check if the location has changed
    if current_location != previous_location:
        sendLocation(current_location)
        previous_location = current_location

    time.sleep(30)  # Sleep for 30 seconds
