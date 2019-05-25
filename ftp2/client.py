import socket
import os
import sys

l = sys.argv
if len(l) != 3:
    print("Invalid number of command line parameters...Aborting")
    sys.exit()

# IP address
IPaddr = l[1]

# port number
try:
    port = int(l[2])
    if port < 0 or port > 65535:
        print("Invalid port number...Aborting")
        sys.exit()
except:
    print("Invalid port number...Aborting")
    sys.exit()

# create server socket
client_sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

# connect to server
try:
    client_sock.connect((IPaddr, port))
except:
    print("Error connecting to server...Aborting")
    sys.exit()

try:
    while 1:
        command = input("$ ").strip()+"\n"
                
        if command in ["s_ls\n", "s_pwd\n"]:
            # do server ls
            client_sock.send(command.encode())
            output = client_sock.recv(1<<14).decode()
            print(output)
            
        if command == "c_ls\n":
            # do client ls
            ls = os.listdir(path='.')
            print(*ls, sep="\t")
            
        elif command == "c_pwd\n":
            # do server pwd
            print(os.path.abspath("."))    
            
        elif command.startswith("s_cd "):
            # do server cd
            client_sock.send(command.encode())
            status = int(client_sock.recv(2).decode())
            if status == 0:
                print("No such directory: ", command[5:])
            elif status == 1:
                print("Inside: ", command[5:])

        elif command.startswith("c_cd "):
            # do client cd
            dirname = command[5:]
            if not os.path.isdir(dirname):
                print("No such directory: ", dirname)
            else:
                os.chdir(dirname)
                print("Inside: ", dirname)
        
        elif command.startswith("put "):
            # send file
            filename = command[4:]
            client_sock.send(command.encode())
            if not os.path.isfile(filename):
                print("No such file: ", filename)
                continue
            try:
                fh = open(filename, "rb")
            except OSError:
                print("Error opening file: ", filename)
                continue
                
            stat = os.stat(filename)
            size = stat.st_size
            client_sock.send((str(size) + '\n').encode())
            client_sock.sendfile(fh)
            fh.close()
            
        elif command.startswith("get "):
            # recieve file
            client_sock.send(command.encode())
            filename = command[4:]
            size = int(client_sock.recv(10).decode())
            if size == -1:
                print("Can't fetch file from server")
                continue
            
            buff = client_sock.recv(size)    
            
            with open(filename, "wb") as fh:
                fh.write(buff)

        elif command == "quit\n":
            client_sock.close()
            break
        else:
            print("Unknown Command")

except KeyboardInterrupt:
    client_sock.close()