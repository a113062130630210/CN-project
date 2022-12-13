import socket
import threading, wave, pyaudio, time, queue

BUFF_SIZE = 65536
client_socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
client_socket.setsockopt(socket.SOL_SOCKET, socket.SO_RCVBUF, BUFF_SIZE)
host_name = socket.gethostname()
host_ip = "127.0.0.1"  #  socket.gethostbyname(host_name)
print(host_ip)
port = 8888


p = pyaudio.PyAudio()
CHUNK = 10 * 1024
stream = p.open(
    format=p.get_format_from_width(2),
    channels=1,
    rate=44100,
    output=True,
    frames_per_buffer=CHUNK,
)

# create socket
message = b"Hello"
client_socket.sendto(message, (host_ip, port))
DATA_SIZE, _ = client_socket.recvfrom(BUFF_SIZE)
DATA_SIZE = int(DATA_SIZE.decode())
q = queue.Queue(maxsize=DATA_SIZE)
cnt = 0

def getAudioData():
    while True:
        frame, _ = client_socket.recvfrom(BUFF_SIZE)
        q.put(frame)
        print("[Queue size while loading]...", q.qsize())

t1 = threading.Thread(target=getAudioData, args=())
t1.start()

DURATION = DATA_SIZE * CHUNK / 44100
print("[Now Playing]... Data", DATA_SIZE, "[Audio Time]:", DURATION, "seconds")
while True:
    frame = q.get()
    stream.write(frame)
    print(
        "[Queue size while playing]...",
        q.qsize(),
        "[Time remaining...]",
        round(DURATION),
        "seconds",
    )
    DURATION -= CHUNK / 44100
client_socket.close()
print("Audio closed")
os._exit(1)
