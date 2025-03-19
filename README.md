# Class Explanation

1) Peer: 
    - Common class for a peer, holds all the necessary attributes of a peer:
        . int sockfd: socket_discriptor (an endpoint where another socket can connect to)
        . 
    
    - Questions:
        . How do implement this with private attributes? How do it affect to other class (MasterPeer)

2) MasterPeer:
    - A peer that is the host of current process, only one peer is a master in a process.
    - This peer can connect to any other peers outside, also can manage & maintain all the connected peers.

