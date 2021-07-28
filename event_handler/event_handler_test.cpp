#include "event_handler.h"

using namespace x_http_server;

int main() {
    EventHandler *event_handler = new SelectEventHandler(100,5,0);
    
}