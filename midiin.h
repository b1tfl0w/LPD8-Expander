#include <iostream>
#include <cstdlib>
#include "rtmidi/RtMidi.h"

std::string controllerName = "LPD8 20:0";
std::string outputPortName = "Midi Through 14:0";

uint8_t ccMemValues[2048];
uint8_t buttonCC[8] = {52, 53, 54, 55, 56, 57, 58, 59};
uint8_t LPD8Display[8] = {127, 0, 0, 0, 127, 0, 0, 0};
uint8_t LPD8DisplayCH[8] = {127, 0, 0, 0, 127, 0, 0, 0};
uint8_t activeUp = 127;
uint8_t activeDn = 0;
uint8_t LPD8page = 0;
uint8_t LPD8pageCH = 0;
uint8_t LPD8preset = 0;
uint8_t LPD8chan = 0;
unsigned int ledPos = 0;
unsigned int ledPosCH = 0;
uint8_t ccStartNum = 20;
bool isFirstButtonPressed = false;
bool isSecndButtonPressed = false;
bool isPageCH = false;

RtMidiIn *midiin = 0;
RtMidiOut *midiout = 0;
RtMidiOut *midiout2 = 0;

std::vector<unsigned char> message;

void initCCValues()
{
    for (int i = 0; i < 2048; i++)
    {
        ccMemValues[i] = 0;
    }
}

void highlightKnobSelector(uint8_t valueCC)
{
    for (int i = 0; i <= 7; i++)
    {
        if ((valueCC / 16) == i)
        {
            LPD8Display[i] = activeUp;
        }
        else
        {
            LPD8Display[i] = activeDn;
        }
        message[0] = 191; //canale midi 16 !!!
        message[1] = buttonCC[i];
        message[2] = LPD8Display[i];
        midiout->sendMessage(&message);
    }
    std::cout << "Led Number = " << (valueCC / 16) + 1 << std::endl;
}

void drawLPD8intrfc()
{
    for (unsigned int i = 0; i <= 7; i++)
    {
        message[0] = 191; //canale midi 16 !!!
        message[1] = buttonCC[i];
        message[2] = LPD8Display[i];
        midiout->sendMessage(&message);
    }
}

void drawLPD8intrfcCH()
{
    for (unsigned int i = 0; i <= 7; i++)
    {
        message[0] = 191; //canale midi 16 !!!
        message[1] = buttonCC[i];
        message[2] = LPD8DisplayCH[i];
        midiout->sendMessage(&message);
    }
}

void fillLPD8intrfcCH(uint8_t valueCC)
{
    unsigned int i = 0;

    if (valueCC >= buttonCC[0] && valueCC <= buttonCC[3]) //check if input value is in upper part
    {
        LPD8pageCH = (valueCC - buttonCC[0]);
        for (i = 0; i <= 3; i++)
        {
            if (LPD8pageCH == i)
            {
                LPD8DisplayCH[i] = activeDn;
            }
            else
            {
                LPD8DisplayCH[i] = activeUp;
            }
        }
        std::cout << "page: " << (unsigned int)LPD8page + 1 << std::endl;
        std::cout << "pageCH: " << (unsigned int)LPD8pageCH + 1 << std::endl;
        std::cout << "preset: " << (unsigned int)LPD8preset + 1 << std::endl;
        std::cout << "midiCH: " << (unsigned int)LPD8chan + 1 << std::endl;
    }
    if (valueCC >= buttonCC[4] && valueCC <= buttonCC[7]) //check if input value is in lower part
    {
        ledPosCH = (valueCC - buttonCC[4]);
        LPD8chan = (4 * LPD8pageCH) + ledPosCH;
        std::cout << "page: " << (unsigned int)LPD8page + 1 << std::endl;
        std::cout << "pageCH: " << (unsigned int)LPD8pageCH + 1 << std::endl;
        std::cout << "preset: " << (unsigned int)LPD8preset + 1 << std::endl;
        std::cout << "midiCH: " << (unsigned int)LPD8chan + 1 << std::endl;
    }
    if (LPD8chan >= LPD8pageCH * 4 && LPD8chan < LPD8pageCH * 4 + 4) //check if preset can be viewed from this page
    {
        for (i = 0; i <= 3; i++)
        {
            if (i == ledPosCH)
            {
                LPD8DisplayCH[i + 4] = activeDn;
            }
            else
            {
                LPD8DisplayCH[i + 4] = activeUp;
            }
        }
    }
    else
    {
        for (i = 0; i <= 3; i++)
        {
            LPD8DisplayCH[i + 4] = activeUp;
        }
    }
    drawLPD8intrfcCH();
}

void fillLPD8intrfc(uint8_t valueCC)
{
    unsigned int i = 0;

    if (valueCC >= buttonCC[0] && valueCC <= buttonCC[3]) //check if input value is in upper part
    {
        LPD8page = (valueCC - buttonCC[0]);
        for (i = 0; i <= 3; i++)
        {
            if (LPD8page == i)
            {
                LPD8Display[i] = activeUp;
            }
            else
            {
                LPD8Display[i] = activeDn;
            }
        }
        std::cout << "page: " << (unsigned int)LPD8page + 1 << std::endl;
        std::cout << "pageCH: " << (unsigned int)LPD8pageCH + 1 << std::endl;
        std::cout << "preset: " << (unsigned int)LPD8preset + 1 << std::endl;
        std::cout << "midiCH: " << (unsigned int)LPD8chan + 1 << std::endl;
    }
    if (valueCC >= buttonCC[4] && valueCC <= buttonCC[7]) //check if input value is in lower part
    {
        ledPos = (valueCC - buttonCC[4]);
        LPD8preset = (4 * LPD8page) + ledPos;
        std::cout << "page: " << (unsigned int)LPD8page + 1 << std::endl;
        std::cout << "pageCH: " << (unsigned int)LPD8pageCH + 1 << std::endl;
        std::cout << "preset: " << (unsigned int)LPD8preset + 1 << std::endl;
        std::cout << "midiCH: " << (unsigned int)LPD8chan + 1 << std::endl;
    }
    if (LPD8preset >= LPD8page * 4 && LPD8preset < LPD8page * 4 + 4) //check if preset can be viewed from this page
    {
        for (i = 0; i <= 3; i++)
        {
            if (i == ledPos)
            {
                LPD8Display[i + 4] = activeUp;
            }
            else
            {
                LPD8Display[i + 4] = activeDn;
            }
        }
    }
    else
    {
        for (i = 0; i <= 3; i++)
        {
            LPD8Display[i + 4] = activeDn;
        }
    }
    drawLPD8intrfc();
}

// fill memory and send cc
void fillCCMemValues(uint8_t ccNum, uint8_t ccValue)
{
    uint8_t ccNumToSend = (8 * LPD8preset) + (ccNum - ccStartNum);
    uint8_t memPos = (128 * LPD8chan) + (8 * LPD8preset) + (ccNum - ccStartNum);
    ccMemValues[memPos] = ccValue;
    message[0] = (176 + LPD8chan); //canale midi 16 !!!
    message[1] = ccNumToSend;
    message[2] = ccValue;
    midiout2->sendMessage(&message);
    std::cout << (int)message[0] << " " << (int)message[1] << " " << (int)message[2] << std::endl;
}

void mycallback(double deltatime, std::vector<unsigned char> *message, void * /*userData*/)
{
    /*    if (message->size() > 10)
        return;
*/
    unsigned int nBytes = message->size();
    for (unsigned int i = 0; i < nBytes; i++)
        std::cout << "Byte " << i << " = " << (int)message->at(i) << ", ";
    if (nBytes > 0)
    {
        std::cout << "stamp = " << deltatime << std::endl;
    }
    // check for knob controllers
    if (message->at(0) == 191 && message->at(1) >= ccStartNum && message->at(1) <= ccStartNum + 7)
    {
        fillCCMemValues(message->at(1), message->at(2));
    }

    if (message->at(0) == 191 && message->at(1) == buttonCC[0] && message->at(2) != 0)
    { // first button is pressed
        isFirstButtonPressed = true;
    }
    if (message->at(0) == 191 && message->at(1) == buttonCC[0] && message->at(2) == 0)
    { // first button is released
        isFirstButtonPressed = false;
    }
    if (message->at(0) == 191 && message->at(1) == buttonCC[3] && message->at(2) != 0)
    { // first button is pressed
        isSecndButtonPressed = true;
    }
    if (message->at(0) == 191 && message->at(1) == buttonCC[3] && message->at(2) == 0)
    { // first button is released
        isSecndButtonPressed = false;
    }

    // check for buttons
    if (message->at(0) == 191 && message->at(1) >= buttonCC[0] && message->at(1) <= buttonCC[7])
    {

        if (isFirstButtonPressed && isSecndButtonPressed)
        {
            if (isPageCH)
            {
                isPageCH = false;
            }
            else
            {
                isPageCH = true;
            }
        }
    }
    if (isPageCH)
    {
        fillLPD8intrfcCH(message->at(1));
    }
    else
    {
        fillLPD8intrfc(message->at(1));
    }
}

bool chooseMidiIn(RtMidiIn *rtmidi);
bool chooseMidiOut(RtMidiOut *rtmidi);
bool chooseMidiOut2(RtMidiOut *rtmidi);

void setupMidi()//called by main.cpp
{
    message.push_back(0);
    message.push_back(0);
    message.push_back(0);
    initCCValues();
    try
    {

        // RtMidiIn constructor
        midiout = new RtMidiOut();

        // Call function to select port.
        if (chooseMidiOut(midiout) == false)
            goto cleanup;
    }
    catch (RtMidiError &error)
    {
        error.printMessage();
    }
    try
    {

        // RtMidiIn constructor
        midiout2 = new RtMidiOut();

        // Call function to select port.
        if (chooseMidiOut2(midiout2) == false)
            goto cleanup;
    }
    catch (RtMidiError &error)
    {
        error.printMessage();
    }

    try
    {

        // RtMidiIn constructor
        midiin = new RtMidiIn();

        // Call function to select port.
        if (chooseMidiIn(midiin) == false)
            goto cleanup;
        // Set our callback function.  This should be done immediately after
        // opening the port to avoid having incoming messages written to the
        // queue instead of sent to the callback function.
        midiin->setCallback(&mycallback);

        // Don't ignore sysex, timing, or active sensing messages.
        midiin->ignoreTypes(false, false, false);
        std::cout << "\nWorking... press <enter> to quit.\n";
        char input;
        std::cin.get(input);
        std::cout << "\nKey pressed: " << input << "\n";
    }
    catch (RtMidiError &error)
    {
        error.printMessage();
    }

cleanup:

    delete midiin;
    delete midiout;
    delete midiout2;
}

bool chooseMidiOut(RtMidiOut *rtmidi) //choose output for controller display
{
    std::string keyHit;

    std::string portName;
    unsigned int i = 0, nPorts = rtmidi->getPortCount();
    if (nPorts == 0)
    {
        std::cout << "No input ports available!" << std::endl;
        return false;
    }
    
    std::cout << "\nChoose a port number for controller: \n\n";
    for (i = 0; i < nPorts; i++)
    {
        portName = rtmidi->getPortName(i);
        std::cout << "  Output port " << i << " " << portName << '\n';
    }

    do
    {
        std::cin >> i;
    } while (i >= nPorts);
    std::getline(std::cin, keyHit); // used to clear out stdin

    rtmidi->openPort(i);
    controllerName = rtmidi->getPortName(i);
    std::cout << "\nAssigned controller: " << controllerName << std::endl;

    return true;
}

bool chooseMidiOut2(RtMidiOut *rtmidi) //choose where to output CCs
{
    std::cout << "\nWould you like to open a virtual output port? [y/N] ";

    std::string keyHit;
    std::getline(std::cin, keyHit);
    if (keyHit == "y")
    {
        rtmidi->openVirtualPort();
        outputPortName = rtmidi->getPortName();
        std::cout << "\nAssigned output port: RtMidi Output Client" << std::endl;
        return true;
    }

    //std::string portName;
    unsigned int i = 0, nPorts = rtmidi->getPortCount();
    if (nPorts == 0)
    {
        std::cout << "No output ports available!" << std::endl;
        return false;
    }
    
    std::cout << "\nChoose a port number for output: \n\n";
    for (i = 0; i < nPorts; i++)
    {
        outputPortName = rtmidi->getPortName(i);
        std::cout << "  Output port " << i << " " << outputPortName << '\n';
    }

    do
    {
        std::cin >> i;
    } while (i >= nPorts);
    std::getline(std::cin, keyHit); // used to clear out stdin

    rtmidi->openPort(i);
    std::cout << "\nAssigned output: " << outputPortName << std::endl;

    return true;
}

bool chooseMidiIn(RtMidiIn *rtmidi)
{
    std::string portName;
    unsigned int i = 0, nPorts = rtmidi->getPortCount();
    if (nPorts == 0)
    {
        std::cout << "No input ports available!" << std::endl;
        return false;
    }

        for (i = 0; i < nPorts; i++)
        {
            portName = rtmidi->getPortName(i);
            //std::cout << "  Input port '" << i << "' " << portName << '\n';
            if (portName == controllerName)
            {
                rtmidi->openPort(i);
                //std::cout << "  assigned in port '" << i << "' " << portName << '\n';
                return true;
            }
        }

    return false;
}
