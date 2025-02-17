#include "src/etherShield.h"

static uint8_t mymac[6] = {0x54, 0x55, 0x58, 0x10, 0x00, 0x24};
static uint8_t myip[4] = {192, 168, 1, 17};

#define BUFFER_SIZE 250
unsigned char buf[BUFFER_SIZE + 1];


uint16_t pkt_len;

EtherShield es = EtherShield();

void eth_init()
{
  /*initialize enc28j60*/
  es.ES_enc28j60Init(mymac);
  es.ES_enc28j60clkout(2); // change clkout from 6.25MHz to 12.5MHz

  delay(10);

  // 0x476 is PHLCON LEDA=links status, LEDB=receive/transmit
  // enc28j60PhyWrite(PHLCON,0b0000 0100 0111 01 10);
  es.ES_enc28j60PhyWrite(PHLCON, 0x476);
  delay(100);

  //init the ethernet/ip layer:
  es.ES_init_ip_arp_udp_tcp(mymac, myip, 80);
}

uint16_t packet_receive()
{
  return es.ES_enc28j60PacketReceive(BUFFER_SIZE, buf);
}

//=================================================================

char send_buff[60];

void setup() {

  Serial.begin(115200);

  eth_init();

}

int count = 0;

void loop() {

  pkt_len = packet_receive();

  if (pkt_len != 0) {
    //    Serial.printf("Destination MAC Address : ");
    //    for (int i = 0; i < 6; i++)
    //    {
    //      Serial.printf("%02x ", buf[i]);
    //    }
    //    Serial.printf("\n");
    //
    //    Serial.printf("Source MAC Address : ");
    //    for (int i = 0; i < 6; i++)
    //    {
    //      Serial.printf("%02x ", buf[i + 6]);
    //    }
    //    Serial.printf("\n");
    //
    //    Serial.printf("Ethernet Type : ");
    //    for (int i = 0; i < 2; i++)
    //    {
    //      Serial.printf("%02x ", buf[i + 12]);
    //    }
    //    Serial.printf("\n");

    if (buf[12] == 0x08 && buf[13] == 0x06)
    {
      //Serial.printf("ARP Packet Received...");

      for (int i = 0; i < 6; i++)
      {
        if (buf[i] == 0xff) count++;
      }
      if (count == 6)
      {
        //        Serial.printf("Broadcasting ARP Packet...\n");
        //        Serial.printf("Hardware Type : ");
        //        Serial.printf("%02x %02x \n", buf[14], buf[15]);
        //        Serial.printf("Protocol Type : ");
        //        Serial.printf("%02x %02x \n", buf[16], buf[17]);
        //        Serial.printf("Hardware Size : ");
        //        Serial.printf("%02x \n", buf[18]);
        //        Serial.printf("Protocol Size : ");
        //        Serial.printf("%02x \n", buf[19]);
        //        Serial.printf("Opcode Type : ");
        //        Serial.printf("%02x %02x \n", buf[20], buf[21]);
        //        Serial.printf("Sender MAC Address: ");
        //        for (int i = 0; i < 6; i++) Serial.printf("%02x ", buf[i + 22]);
        //        Serial.printf("\n");
        //        Serial.printf("Sender MAC Address: ");
        //        for (int i = 0; i < 4; i++) Serial.printf("%d. ", buf[i + 28]);
        //        Serial.printf("\n");
        //        Serial.printf("Target IP Address: ");
        //        for (int i = 0; i < 6; i++)Serial.printf("%02x ", buf[i + 32]);
        //        Serial.printf("\n");
        //        Serial.printf("Target MAC Address: ");
        //        for (int i = 0; i < 4; i++)Serial.printf("%d. ", buf[i + 38]);
        //        Serial.printf("\n");

        count = 0;

        for (int i = 0; i < 4; i++)
        {
          if (buf[38 + i] == myip[i]) count++;
        }
        if (count == 4)
        {
          count = 0;
          Serial.printf("ARP Packet is mine \n");
          for (int i = 0; i < 6; i++) send_buff[i] = buf[i + 22];
          for (int i = 0; i < 6; i++) send_buff[i + 6] = i;
          send_buff[12] = 0x08;
          send_buff[13] = 0x06;
          es.packet_send(60, send_buff);
        }
      }

    }
  }

  //delay(1);
}
