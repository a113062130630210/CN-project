# restart ./server every hour

import time
from subprocess import *

log = open("restart.log", "w")

while True:
    p = Popen(["./server"], stdout=log, stderr=log)
    print("server started")
    
    time.sleep(60 * 60)
    
    p.kill()
    print("server killed")
