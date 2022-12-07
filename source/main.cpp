#include "MicroBit.h"
#include "MicroBitUARTService.h"

MicroBit uBit;

int cpt = 0;

// ================================================================================

void onButton(MicroBitEvent e)
{
    if (e.source == MICROBIT_ID_BUTTON_A)
    {
        uBit.display.scroll(cpt, 50);
    }

    if (e.source == MICROBIT_ID_BUTTON_B)
    {
        uBit.serial.printf("BUTTON B: ");
        uBit.display.scroll("B");
        // uBit.display.print("B");
    }   

    uBit.serial.printf("\r\n");
}

// ================================================================================

void onSerial(MicroBitEvent)
{
    ManagedString rec = uBit.serial.readUntil(ManagedString("\n"), ASYNC);

    uBit.radio.datagram.send(rec);

    cpt++;
}

char *decrypt(const char *encrypted)
{
    char *decrypted = new char[strlen(encrypted) + 1];
    char *begin = decrypted;

    while (*encrypted != '\0')
    {
        *decrypted++ = *encrypted++ - 3;
    }
    *decrypted = '\0';

    return begin;
}

char *encrypt(const char *decrypted)
{
    char *encrypted = new char[strlen(decrypted) + 1];
    char *begin = encrypted;

    while (*decrypted != '\0')
    {
        *encrypted++ = *decrypted++ + 3;
    }
    *encrypted = '\0';

    return begin;
}

void onData(MicroBitEvent)
{
    PacketBuffer rec(4);

    rec = uBit.radio.datagram.recv();

    uBit.serial.send(rec);
}

int main()
{
    // Initialise the micro:bit runtime.
    uBit.init();

    uBit.display.scroll("START", 50);

    uBit.serial.baud(115200);

    uBit.messageBus.listen(MICROBIT_ID_SERIAL, MICROBIT_SERIAL_EVT_DELIM_MATCH, onSerial);
    uBit.serial.eventOn(ManagedString("\n"), ASYNC);
    uBit.serial.read(ASYNC);

    uBit.messageBus.listen(MICROBIT_ID_RADIO, MICROBIT_RADIO_EVT_DATAGRAM, onData);

    // Create a message bus that listens to button events
    uBit.messageBus.listen(MICROBIT_ID_BUTTON_A, MICROBIT_BUTTON_EVT_CLICK, onButton);
    // uBit.messageBus.listen(MICROBIT_ID_BUTTON_B, MICROBIT_BUTTON_EVT_CLICK, onButton);

    uBit.radio.enable();
    uBit.radio.setGroup(5);

    // Init du message chiffré
    // char *cipher = NULL;

    while (true)
    {
        PacketBuffer buffer(4);

        buffer[0] = 'a';
        buffer[1] = 'b';
        buffer[2] = 'c';
        buffer[3] = 'd';

        uBit.radio.datagram.send(buffer);

        uBit.sleep(2000);
    }

    release_fiber();
}