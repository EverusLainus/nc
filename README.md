# nc

a simple client server program 

## nc_client.c

Client reads message from console and sends it to the server and waits to receive a message from the Server continuously.

## nc_server.c

Server sends a hardcoded message that says "something magical is gonna happen you" to the every Client program that connects to it.

## nc_main.c

main receives four or five arguments.
If it receives four arguments, it should be of form <file> <flag1> <ip> <port>. main checks the second argument to decide if it is gonna act like a server or client.The flag1 is set to be -c for a client program and -s for a server program. 

If it receives five arguments, it should be of form <file> <flag1> <flag2> <ip> <port>. main checks for the third argument to decide if it is a webserver. The flag2 is set to be -w for a web server.

The main return "not enough arguments" if it doesnt have enough arguments.

# build

```
cd nc 
make
```