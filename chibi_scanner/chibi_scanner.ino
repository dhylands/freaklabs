/* chibiArduino Wireshark Bridge, Example 9
This sketch enables promiscuous mode on the Freakduino boards and dumps
the raw 802.15.4 frames out to the serial port. It should be used in
conjunction with the FreakLabs "wsbridge" application to feed the raw
frames into Wireshark.

Before using this sketch, please go into the chibiUsrCfg.h file and
enable promiscuous mode. To do this, change the definition:

#define CHIBI_PROMISCUOUS 0
to
#define CHIBI_PROMISCUOUS 1

When not using promiscuous mode, please disable this setting by changing
it back to 0.
*/

#include <chibi.h>
#include <elapsedMillis.h>

// Setting Promiscuous mode causes the full packets to be returned,
// whether they're addressed to us or not. If promiscuous mode is
// set to zero, then we only get the data portion of the packets
// and not the headers.
//
// For example, suppose a node is calling: chibiTx(BROADCAST_ADDR, "Hello World", 12);
//
// With promiscuous mode set to 0, then calling chibiGetData will return 12 bytes:
//
//    48 65 6c 6c 6f 20 57 6f 72 6c 64 00
//
// With promiscuous mode set to 1, then calling chibiGetData will return 23 bytes:
//
//    17 41 98 ce 34 12 ff ff 03 00 48 65 6c 6c 6f 20 57 6f 72 6c 64 00 e7
//
// Note that when promiscuous mode = 1, the first byte of the received data is the
// length of the data (including the length byte).

#if CHIBI_PROMISCUOUS != 1
#error Must set CHIBI PROMISCUOUS to 0 (in chibiUsrCfg.h)
#endif

#define   MIN_CHANNEL 11
#define   MAX_CHANNEL 26
#define   DWELL_MSECS 3000

#define   STATE_LEN   0
#define   STATE_DATA  1

#define LED 4

elapsedMillis gDwellMillis;
uint8_t gChannel;
uint32_t gPacketCount;

/**************************************************************************/
// Set the channel that's being used.
/**************************************************************************/
void setChannel(uint8_t channel)
{
  if (channel > MAX_CHANNEL) {
    channel = MIN_CHANNEL;
  }
  gChannel = channel;
  chibiSetChannel(gChannel);
  gPacketCount = 0;
  gDwellMillis = 0;
}

/**************************************************************************/
// Initialize
/**************************************************************************/
void setup()
{
  // Init the chibi stack
  chibiInit();

  Serial.begin(115200);

  // Blink the LED 2 times to indicate we're running the scanner
  pinMode(LED, OUTPUT);

  for (int i = 0; i < 2; i++) {
    digitalWrite(LED, 1);
    delay(100);
    digitalWrite(LED, 0);
    delay(100);
  }

  // Set the channel to match the channel used by the Zigbee controller.
  setChannel(MIN_CHANNEL);
}

/**************************************************************************/
// Loop
/**************************************************************************/
void loop()
{
  if (gDwellMillis > DWELL_MSECS) {
    // Print the summary
    char msg[40];
    snprintf(msg, sizeof(msg), "Channel %2d: packets: %4d", gChannel, gPacketCount);
    Serial.println(msg);

    // Switch to the next channel
    setChannel(gChannel + 1);
  }

  // Check if any data was received from the radio. If so, then handle it.
  if (chibiDataRcvd() == true)
  {
    int len;
    byte buf[CHB_MAX_PAYLOAD];

    // send the raw data out the serial port in binary format
    len = chibiGetData(buf);

    gPacketCount += 1;
  }
}
