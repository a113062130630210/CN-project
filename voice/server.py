import math
import socket
import sys
import time
import wave

import pyaudio

if len(sys.argv) < 4:
    print("Usage: python server.py <host IP> <port> <file>")
    print("You can use ifconfig to find your host IP")
    sys.exit(1)

BUFF_SIZE = 65536
server_socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
server_socket.setsockopt(socket.SOL_SOCKET, socket.SO_RCVBUF, BUFF_SIZE)
host_name = socket.gethostname()
host_ip = sys.argv[1]
port = int(sys.argv[2])
server_socket.bind((host_ip, (port)))
print("server listening at", (host_ip, (port)))

CHUNK = 10 * 1024
wf = wave.open(sys.argv[3])
p = pyaudio.PyAudio()
stream = p.open(
    format=p.get_format_from_width(wf.getsampwidth()),
    channels=wf.getnchannels(),
    rate=wf.getframerate(),
    input=True,
    frames_per_buffer=CHUNK,
)
print(wf.getframerate(), wf.getnchannels(), wf.getsampwidth())

data = None
sample_rate = wf.getframerate()

msg, client_addr = server_socket.recvfrom(BUFF_SIZE)
print("Connect to", client_addr, msg)
DATA_SIZE = math.ceil(wf.getnframes() / CHUNK)
DATA_SIZE = str(DATA_SIZE).encode()
print("[Sending data size]...", wf.getnframes() / sample_rate)
server_socket.sendto(DATA_SIZE, client_addr)

cnt = 0
while cnt <= (wf.getnframes() / CHUNK):
    data = wf.readframes(CHUNK)
    server_socket.sendto(data, client_addr)
    time.sleep(0.1)
    print(f"{cnt} / {wf.getnframes() / CHUNK}")
    cnt += 1
