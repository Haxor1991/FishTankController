//
// Created by Cory King on 3/18/17.
//

#include "ControllerEvent.h"

void ControllerEvent::setActiveState(bool isActive)  {
    if(this->eventActive != isActive) {
        Serial.println("Change in active state!");
        this->eventActive = isActive;
        if(this->eventActive) {
            Serial.println("Firing onEventActive");
            this->onEventActive();
        } else {
            Serial.println("Firing onEventInactive");
            this->onEventInactive();
        }
    }
}

bool ControllerEvent::isEventActive() {
    return this->eventActive;
}

ControllerEventList &ControllerEventList::setEvent(Chronos::EventID eventID, eventAction onEventActive, eventAction onEventInactive) {
    this->operator[](eventID) = new ControllerEvent(onEventActive, onEventInactive);
    return *this;
}

ControllerEvent *ControllerEventList::getEvent(Chronos::EventID eventID) {
    return this->operator[](eventID);
}
