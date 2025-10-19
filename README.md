1 - Compile serveur and client
```bash
make
```
2 - Listen on tcpdump (1º terminal)
```bash
sudo tcpdump -i lo -s 0 -w capture.pcap tcp port 8080
```
3 - Init server (2º terminal)
```bash
./serveur
```
4 - Init client (3º terminal)
```bash
./client
```
5 - Exit tcpdump and after visualizate on wireshark
```bash
wireshark capture.pcap
```