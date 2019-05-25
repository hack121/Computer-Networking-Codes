import os 
import signal
while True:
    try:
        print("$", end="")
        s = input()
        os.system(s)
    except EOFError:
        exit()
    except KeyboardInterrupt:
        os.kill(os.getpid(), signal.SIGKILL)  
