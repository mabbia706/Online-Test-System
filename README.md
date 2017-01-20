# Online-Test-System
An Online Test System which contains three types of Data Audio, Video, Images.

##Details:
Whenever a client connects to the server, it will display three kinds of tests (image, audio and video). The main server does not want to overburden itself by listening to all incoming clients and handle the tests, so it only stores the info about which sub-server contains what sort of test. When a sub-server connects to the main server it sends the main server a file containing the list of subjects of tests it has. When a client connects to a server it has to the choose the type of test and subject of test. The main server searches the records and sends back the appropriate test.

##Working:
A Main Server in C.
A Client with Graphical User Interface.
A TCP Sub-server. When a sub-server connects to the main server it sends it the list of the tests it has using UDP. The format of the file sub-server has is:
subject of the test, type of test , size 
The list a sub-server send to the main server will have the following format:
Test_name, file_size, test_type, sub-server IP, sub-server-port
There should are 3 instances of sub-server

Main server makes a database(file) with all the data it gets from the sub-servers.
Client sends request to this server through this GUI using UDP. 
e.g Client sends “Mathematics & Written test”
Main server searches the record in the database it has and sends these search results to the client, along with the IP and port number of the sub-server to which that respective search result belongs to.
The Client shows these results in GUI. It does not show the IP and Port of the sub-server as it is for internal use.
The client selects a search result of its own choice and develops a tcp connection with the sub server to which that search result belongs to and start downloading the file or streaming the audio/video (IP and Port number were sent by the server to the client earlier!).

Since the file can be too large it has to be broken down into chunks. 
The client will receive those chunks and upon receiving. Client will play the audio/video or show written test and user will send the answer to server. Server will store this answer along with subject name and question number and client id.

##How to run?

1. For this Ubuntu/linux is needed.
2. First run the main server then run sub servers using terminal.
3. Open the java client in IntelliJ IDE and run the client from their.


