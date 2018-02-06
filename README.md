Write a daemon-like program that will
 serve information about the device to clients via TCP. The program will be named "infod" and will listen on all interfaces on port 40000.

The protocol will be a client-serve, command-request one:
- client connects to the server via TCP
- client sends a request in the form specified below
- server retrieves the information and sends back the response to the client
- [other requests]
- client disconnects

Request packet:

int reqId | int argLen
 | argData

reqId - the id of the requested information
 (ex: cpu info, memory info, interface status ...)

argLen - number of bytes forming the
 argument(s) to the request (argData). The arguments are specific for each request. If 0, no arguments will be given and the argData field will be empty

argData - [optional] the arguments
 for the request. Example: for interface status the argument will be the ifIndex (interface index): argLen = 4, argData = ifIndex (4 bytes)

Server packet:

int reqId | int errCode
 | int respLen | respData

reqId - the request id echoed back

errCode - 0 no error, 100 request
 id not implemented, .... Only if errCode is zero the packet contains respLen and respData

respLen - [optional] number of bytes
 that make up the response data. This is specific for each request type and must not be 0.

respData - [optional] the reponse
 data for the request.

The requests supported by the server
 should be at least:

   meminfo
 (memory information)

   reqId = 1

   no arguments

   response data: total memory,
 free memory (see /proc/meminfo)



   cpuinfo
 (cpu usage for the last second)

   reqId = 2

   no arguments

   response data: percentage
 of cpu utilization



Notes:

It is ok if the server handles
 one client at a time. It is not required to handle multiple connections at the same time.

If you want to change the protocol
 to make it better, or implement more requests, be my guest.

Besides the actual daemon,
 a client application should also be written.

Future improvements:

Think about what it would take
 to make the server handle multiple clients (concurrent server), instead of handling one at a time (iterative server)
