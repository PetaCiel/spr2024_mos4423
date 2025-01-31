#include "Arduino.h"
#include "MotorDriver.h"


void MDOUT::setA(int enb1, int enb2, int phase) {
    this->a_enb1 = enb1;
    this->a_enb2 = enb2;
    this->a_phase = phase;
    pinMode(this->a_enb1 , OUTPUT);
    pinMode(this->a_enb2 , OUTPUT);
    pinMode(this->a_phase , OUTPUT);
}
void MDOUT::setB(int enb1, int enb2, int phase) {
    this->b_enb1 = enb1;
    this->b_enb2 = enb2;
    this->b_phase = phase;
    pinMode(this->b_enb1 , OUTPUT);
    pinMode(this->b_enb2 , OUTPUT);
    pinMode(this->b_phase , OUTPUT);
}
void MDOUT::stby(int stby) {
    this->stpin = stby;
    pinMode(this->stpin , OUTPUT);
    digitalWrite(this->stpin,HIGH);
}

void MDOUT::stop(bool flag,int sval) {
    if (flag == true) {
        this->m_enb1 = this->a_enb1;
        this->m_enb2 = this->a_enb2;
        this->m_phase = this->a_phase;
    }
    else{
        this->m_enb1 = this->b_enb1;
        this->m_enb2 = this->b_enb2;
        this->m_phase = this->b_phase;
    }
    digitalWrite(m_enb1, LOW);
    digitalWrite(m_enb2, LOW);
    analogWrite(m_phase,sval);
}
void MDOUT::forw(bool flag,int sval) {
    if (flag == true) {
        this->m_enb1 = this->a_enb1;
        this->m_enb2 = this->a_enb2;
        this->m_phase = this->a_phase;
    }
    else{
        this->m_enb1 = this->b_enb1;
        this->m_enb2 = this->b_enb2;
        this->m_phase = this->b_phase;
    }
    digitalWrite(m_enb1, LOW);
    digitalWrite(m_enb2, HIGH);
    analogWrite(m_phase,sval);
}
void MDOUT::revr(bool flag,int sval) {
    if (flag == true) {
        this->m_enb1 = this->a_enb1;
        this->m_enb2 = this->a_enb2;
        this->m_phase = this->a_phase;
    }
    else{
        this->m_enb1 = this->b_enb1;
        this->m_enb2 = this->b_enb2;
        this->m_phase = this->b_phase;
    }
    digitalWrite(m_enb1, HIGH);
    digitalWrite(m_enb2, LOW);
    analogWrite(m_phase,sval);
}
void MDOUT::brak(bool flag) {
    if (flag == true) {
        this->m_enb1 = this->a_enb1;
        this->m_enb2 = this->a_enb2;
        this->m_phase = this->a_phase;
    }
    else{
        this->m_enb1 = this->b_enb1;
        this->m_enb2 = this->b_enb2;
        this->m_phase = this->b_phase;
    }
    digitalWrite(m_enb1, HIGH);
    digitalWrite(m_enb2, HIGH);
}

