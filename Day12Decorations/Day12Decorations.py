import json

def processList(inList, valueProcessor):
	sum = 0
	for v in inList:
		sum += valueProcessor(v)
	return sum
	
def processDict(inDict, valueProcessor):
	# Added for test 2, to filter red objects.
	for k, v in inDict.items():
		if type(v) is unicode and v == "red":
			return 0
			
	sum = 0
	for k, v in inDict.items():
		sum += valueProcessor(v)
	return sum
	
def processValue(v):
	if type(v) is int:
		return v
	if type(v) is unicode:
		return 0
	elif type(v) is list:
		return processList(v, processValue)
	elif type(v) is dict:
		return processDict(v, processValue)
	else:
		print("Unhandled json object entry, type = " + str(type(v)))
		return 0
	
jsonObj = { }
with open("input.txt") as f:
	jsonObj = json.loads(f.read())

sum = processValue(jsonObj)
print("Num decorations that don't count anything red = " + str(sum))

