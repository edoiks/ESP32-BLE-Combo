#pragma once


void setValueInDescriptor(uint8_t* hidDescriptor, int& possition, uint8_t field, uint8_t value)
{
    hidDescriptor[possition++] = field;
    hidDescriptor[possition++] = value;
}

void setValueInDescriptor(uint8_t* hidDescriptor, int& possition, uint8_t field)
{
    hidDescriptor[possition++] = field;
}