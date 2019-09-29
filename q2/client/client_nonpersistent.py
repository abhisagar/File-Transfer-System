#client.py
 
import sys
import socket 
 
while True: 
    skClient = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    skClient.connect(("127.0.0.1",2525))

    sFileName = raw_input("Enter Filename to download from server : ")
    sData = "Temp"
     
    skClient.send(sFileName)
    if sFileName == "END":
        val=1
        break    
    
    sData = skClient.recv(1024)
    fDownloadFile = open(sFileName,"wb")
    while sData:
        fDownloadFile.write(sData)
        sData = skClient.recv(1024)
    print "Download Completed"
     
    skClient.close()
