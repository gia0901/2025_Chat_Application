***CHAT APPLICATION***
# How to run this app


# Class Explanation

1) Peer: 
    - Common class for a peer, holds all the necessary attributes of a peer:
        . int sockfd: socket_discriptor (an endpoint where another socket can connect to)
        . 
    
    - Questions:
        . How do implement this with private attributes? How does it affect to other class (MasterPeer)?

2) MasterPeer:
    - A peer that is the host of current process, only one peer is a master in a process.
    - This peer can connect to any other peers outside, also can manage & maintain all the connected peers.




99) Miscellaneous
    1. What is the relationship of IPv4 address & port? What does each other do?
        . IPv4 address tell us which device to communicate with.
        . Port numbers tell us which application/service to communicate on that device.

        Example: A host server with IPv4 address: 192.168.7.1. This server can run:
                    + A web server on port 80 (HTTP)
                    + A database server on port 3306 (MySQL)
                    + An SSH service on port 22
