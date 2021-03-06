//
// Created by Cory King on 2/18/17.
//

#include "pump.h"
#define max(a,b) ((a)>(b)?(a):(b))
//#define PUMP_DEBUG false

void Pump::OnUpdate(uint32_t deltaTime){
    Task::OnUpdate(deltaTime);
    uint32_t amountPumped = 0;
    if(getMotorState() == MotorState::ACTIVE) {
        amountPumped = NlPerMs * deltaTime;
        this->requestedAmountNl = max(0, (int32_t)(this->requestedAmountNl-amountPumped));
#ifdef PUMP_DEBUG
            Serial.print(this->requestedAmountNl);
            Serial.print(" ");
            Serial.print(deltaTime);
            Serial.print(" ");
            Serial.println(amountPumped);
#endif
        if(this->requestedAmountNl <= 0) {
            Serial.print(this->getDeviceName());
            Serial.println(": Setting to idle...");
            currentState = MotorState::IDLE;
            if(this->amountDispensedFn != NULL) {
                this->amountDispensedFn(this);
            }
        }
    }
    if(getMotorState() == MotorState::FORCE_ACTIVE) {
        amountPumped = NlPerMs * deltaTime;
    }
    if(getMotorState() == MotorState::IDLE) {
        if(this->requestedAmountNl > 0) {
            Serial.print(this->getDeviceName());
            Serial.print(": Setting %s to active (");
            Serial.print(this->requestedAmountNl);
            Serial.println(" nL left to pump)");
            currentState = MotorState::ACTIVE;
            dispensedAmountNl = 0;
        }
    }
    dispensedAmountNl += amountPumped;
}

Pump::Pump(float_t mlPerS, String deviceName) : Task(MsToTaskTime(20)), ShiftDevice(deviceName) {
    this->NlPerMs = mlPerS * 1000.0; // mL / S * 1S / 1000mS * 1000uL/1mL * 1000nL/1uL = nL/mS
    this->requestedAmountNl=0;
}

void Pump::setMlPerS(float_t mlPerS) {
    this->NlPerMs = mlPerS * 1000.0;
}

void Pump::startDispenser() {
    if(!isDispensing()) {
        dispensedAmountNl = 0;
    }
    currentState = MotorState::FORCE_ACTIVE;
}

void Pump::stopDispenser() {
    currentState = MotorState::IDLE;
    this->requestedAmountNl=0;
}

bool Pump::isDispensing() {
    return this->getDeviceState();
}

void Pump::toJson(JsonObject &obj) {
    obj["name"] = this->getDeviceName();
    obj["isDispensing"]= this->isDispensing();
    obj["NLPerMs"] = String(this->NlPerMs);
    obj["dispensedAmountNl"] = this->getAmountDispensedNl();
    obj["requestedAmountNl"] = this->requestedAmountNl;
    obj["currentState"] = this->currentState;
}

Pump::Pump(float mlPerS, String pumpName, AmountDispensedFn amountDispensedFn) : Pump(mlPerS, pumpName) {
    this->amountDispensedFn = amountDispensedFn;
}

void Pump::setAmountDispensedFn(AmountDispensedFn amountDispensedFn) {
    Pump::amountDispensedFn = amountDispensedFn;
}
