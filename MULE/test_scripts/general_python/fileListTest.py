import os

dataFiles = "/home/debian/Maria/dataFiles/"
dataFileList = "/home/debian/Maria/groundFiles/filesList.txt"

files = os.listdir(dataFiles)
print(files)
with open(dataFileList,'w') as fileList:
    for line in files:
        fileList.write(line)
        fileList.write("\n")
        print(line)
    fileList.close()
os.remove(dataFileList)