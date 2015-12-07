import hashlib

key = "yzbqklnj"
prefix = "000000"

number = 0
searching = True

while (searching):
    input = key + str(number)

    mdHash = hashlib.md5()
    mdHash.update( input.encode() )
    result = mdHash.hexdigest()

    if result.find(prefix) == 0:
        searching = False
    else:
        number = number + 1

print("Lowest number = " + str(number))
