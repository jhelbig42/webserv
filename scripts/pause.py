
from datetime import datetime

import time

seconds = 4
print("HI! This script will not send anything else for another ", seconds, " seconds...", flush=True)
#flush forces this to print before the sleep, instead of storing in a buffer to be sent with the next print
time.sleep(seconds)
print(seconds, " seconds has passed.")