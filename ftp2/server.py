import socket
import os
import sys

l = sys.argv
if len(l) != 2:
    print("Invalid number of command line parameters...Aborting")
    sys.exit()

# port number
try:
    port = int(l[1])
    if port < 0 or port > 65535:
        print("Invalid port number...Aborting")
        sys.exit()
except:
    print("Invalid port number...Aborting")
    sys.exit()

# create server socket
server_sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

# bind to addres
try:
    server_sock.bind(("0.0.0.0", port))
except:
    print("Error binding to address...Aborting")
    sys.exit()

# listen for connections
try:
    server_sock.listen(5)
except:
    print("Error listening for connections...Aborting")
    sys.exit()

try:
    while 1:
        client, _ = server_sock.accept()
        while 1:
            command = client.recv(50).decode().strip()
            
            if command == "s_ls":
                # do server ls
                ls = os.listdir(path='.')
                client.send(("\t".join(ls) + "\n").encode())
            
            elif command == "s_pwd":
                # do server pwd
                client.send((os.path.abspath(".") + "\n").encode())
            
            elif command.startswith("s_cd "):
                # do server cd
                dirname = command[5:]
                if not os.path.isdir(dirname):
                    client.send(b"0\n")
                else:
                    os.chdir(dirname)
                    client.send(b"1\n")
            
            elif command.startswith("put "):
                # recieve file
                filename = command[4:]
                
                size = int(client.recv(10).decode())
                buff = client.recv(size)
                
                with open(filename, "wb") as fh:
                    fh.write(buff)

            elif command.startswith("get "):
                # send file
                filename = command[4:]
                if not os.path.isfile(filename):
                    client.send(b"-1\n")
                    continue
                try:
                    fh = open(filename, "rb")
                except OSError:
                    client.send(b"-1\n")
                    continue
                
                stat = os.stat(filename)
                size = stat.st_size
                client.send((str(size) + '\n').encode())
                client.sendfile(fh)
                fh.close()
            
            elif command == "quit":
                client.close()
                break

except KeyboardInterrupt:
    server_sock.close()