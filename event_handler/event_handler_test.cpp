#include <iostream>
#include "event_handler.h"

using namespace x_http_server;

void err_sys(const char *s) {
    perror(s);
    exit(1);
}

int main() {
    // EventHandler *event_handler = new SelectEventHandler(100,5,0);
    // EventHandler *event_handler = new PollEventHandler(100,500);
    EventHandler *event_handler = new EPollEventHandler(100,500);
    event_handler->InitEventHandler();
    event_handler->AddReadEvent(0);
    event_handler->AddWriteEvent(1);
    if(event_handler->WaitForEvent() == STATUS_OK) {
        Event ready_event;
        while(event_handler->GetNextReadyEvent(ready_event)) {
            std::cout<<ready_event.fd<<std::endl;
            if(event_handler->CanRead(ready_event)) {
                event_handler->CloseReadEvent(ready_event.fd);
            }
            if(event_handler->CanWrite(ready_event)) { 
                event_handler->CloseWriteEvent(ready_event.fd);
            }
        }
    } else {
        err_sys("wait for events error");
    }
}