### Solace C++ Publisher & Subscriber

A minimal, self‑contained example demonstrating how to build a C++ publisher and subscriber using the Solace C API (solclient).
The subscriber listens on the topic fx/rates/normalized, and the publisher sends a sample FX payload (EURUSD=1.1034) to the same topic.

### Overview
This project contains two standalone executables:

subscriber — connects to Solace, subscribes to a topic, and prints received messages

publisher — connects to Solace and publishes a binary‑attachment message to a topic

### Both programs use:

solClient_initialize for API setup

context with internal I/O thread

callbacks for events and messages

binary attachment payloads

topic-based publish/subscribe

## Requirements
Solace PubSub+ Broker (local or remote) , I have setup on Docker on Mac

Solace C API (solclient) installed (Be sure about the api version and compatibility, this is tested with solclient_Darwin-universal2_opt_7.33.1.1 )

C++ compiler (g++/clang++)
Linux or WSL recommended
.
├── README.md
├── publisher.cpp
└── subscriber.cpp

### Build Instructions
1. Set Solace library paths
Example (adjust to your installation):

bash
export LD_LIBRARY_PATH=/usr/sw/solclient/lib:$LD_LIBRARY_PATH
export CPATH=/usr/sw/solclient/include
2. Compile subscriber
bash
gclang++ subscriber.cpp -std=c++20 \ 
  -I /Users/$USER/solace-c/include \
  -L /Users/$USER/solace-c/lib \
  -lsolclient \
  -Wl,-rpath,/Users/$USER/solace-c/lib \
  -o subscriber

3. Compile publisher
bash
 clang++ publisher.cpp -std=c++20 \ 
  -I /Users/$USER/solace-c/include \
  -L /Users/$USER/solace-c/lib \
  -lsolclient \
  -Wl,-rpath,/Users/$USER/solace-c/lib \
  -o publisher

### Running the Programs
1. Start the subscriber
bash
./subscriber
You should see:

Output:
./subscriber 
Initializing Solace API...
solClient_initialize rc = 0
context_create rc = 0
session_create rc = 0
session pointer = 0x4000ffe
[Event] Session up
session_connect rc = 0
topicSubscribe rc = 0
Listening on topic fx/rates/normalized...

2. Run the publisher
bash
./publisher

Output:
./publisher 
Connected. Publishing messages...
Message sent: EURUSD=1.1034

3. Subscriber output

[Callback] messageCallback fired
[Message] Received: EURUSD=1.1034

### Solace Queue
<img width="927" height="301" alt="image" src="https://github.com/user-attachments/assets/1b00bfe3-f475-4029-b4de-b5cacdaa5f59" />

### Solace Topic
<img width="927" height="301" alt="image" src="https://github.com/user-attachments/assets/4534731b-75be-4b87-b91a-b0b5cc7b0d48" />


### How It Works
Subscriber
Initializes Solace API

Creates a context with an internal thread

Creates a session with message + event callbacks

Subscribes to topic fx/rates/normalized

Prints incoming binary payloads

Publisher
Initializes API and creates session

Allocates a message

Sets topic destination

Attaches binary payload

Sends message via session

### Notes
Queue subscriptions require Solace API 8.x+; this example uses topic subscription for compatibility.

The subscriber uses an internal Solace I/O thread, so no manual event loop is required.

Payloads are handled as binary attachments, which is the recommended pattern for raw text or encoded data.
