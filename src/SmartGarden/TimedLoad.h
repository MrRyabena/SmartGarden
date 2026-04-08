#pragma once

#include <shs_Load.h>
#include <shs_LoadSwitch.h>
#include <shs_Process.h>
#include <shs_ProgramTimer.h>

namespace shs
{
    class TimedLoad;
}


class shs::TimedLoad: public shs::LoadSwitch, public shs::Process
{
public:
    explicit TimedLoad(shs::t::shs_pin_t pin) : shs::LoadSwitch(pin), m_timer(0) {}

    void on(const uint16_t value = UINT16_MAX) override
    {
        shs::LoadSwitch::on();
        m_value = value;
        m_timer.setTimeout(value);
        m_timer.reset();
    }

    void off() override
    {
        shs::LoadSwitch::off();
        m_value = 0;
    }

    void start() override {}
    void tick() override
    {
        if (m_value != 0 && m_timer.expired())
        {
            off();
        }
    }
    void stop() override {}
      
protected:
    shs::ProgramTimer m_timer;
};


