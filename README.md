# About
Boost::Asio client (singleton) static library

## Dependencies
Spdlog  
Boost (tested on 1.72.0)

## Usage
```c++
#include "clientcore/client.h"

int main() {
    std::string ping_string = "ping string";
    const int ping_delay = 10;

    auto& cl = Client::instance();
    cl.set_ping_delay(ping_delay);
    cl.set_ping(ping_string);
    cl.connect("127.0.0.1", 1234);
    
    cl.send_msg("test message");
}
```
