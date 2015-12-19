import json

jsonObj = {}
with open("input.txt", "r") as f:
	jsonData = f.read()
	jsonObj = json.loads(jsonData)

#print(str(jsonObj))

for key, value in jsonObj.items():
	if type(value) is str:
		print("String: " + key)
	elif type(value) is dict:
		print("Dict: " + key)
	elif type(value) is list:
		print("List: " + key)
	else:
		print("Not sure: " + key)