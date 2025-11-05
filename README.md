1 - Compile main
```bash
make
```
2 - Listen on tcpdump (1º terminal)
```bash
sudo tcpdump -i lo -s 0 -w capture.pcap tcp port 8080
```
3 - Init server (2º terminal)
```bash
./main
```
4 - Requistion http on web app
```bash
http://127.0.0.1:8080/
```
5 - Exit tcpdump and after visualizate on wireshark
```bash
wireshark capture.pcap
```