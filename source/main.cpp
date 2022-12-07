#include "MicroBit.h"
#include "MicroBitUARTService.h"

MicroBit uBit;

char prefix = 'b';
char message[4] = "LT";

int cpt = 0;

int connected = 0;

ManagedString rec; 

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
    rec = uBit.serial.readUntil(ManagedString("\n"), ASYNC);

    // Write the data back to the serial port
    // uBit.serial.printf("%d : %s\r \n", cpt, rec.toCharArray());

    // uBit.display.scroll(cpt, 25);

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
    // Message reçu par RF
    ManagedString message = uBit.radio.datagram.recv();

    char first_decrypted = decrypt(message.toCharArray())[0];
    
    // Si le premier caractère est 'b', on déchiffre le message et on le scroll
    if (first_decrypted == prefix)
    {
        // uBit.display.scroll(decrypt(message.toCharArray()));
        uBit.serial.printf("%s\r \n", decrypt(message.toCharArray()));
    }

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
        // Chiffrement du message
        // cipher = encrypt(message);

        // uBit.radio.datagram.send(cipher);

        // uBit.display.scroll(message);

        uBit.sleep(2000);
    }

    release_fiber();
}