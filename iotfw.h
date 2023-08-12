#ifndef _IOTFW_H_
#define _IOTFW_H_

#include <WiFi.h>
#include <WiFiClientSecure.h>

#define USESECURE 1
#ifdef USESECURE
  #include <HTTPS_Server_Generic.h>
//using namespace httpsserver;
  typedef std::function<void(void)> THandlerFunction;
  typedef std::function<String(String)> usrTokFunction;
  void iotfw_send(int code, const String& type, const String& cont);
  void iotfw_sendContent(const String& str);
  void iotfw_sendContent(const uint8_t* buf, size_t len);
  bool iotfw_hasArg(httpsserver::HTTPRequest * req, const String & key);
  void setContentLength(int len);
  void handleFileUpload(httpsserver::HTTPRequest * req, httpsserver::HTTPResponse * res);
  void handle_indirectRoot(httpsserver::HTTPRequest * req,  httpsserver::HTTPResponse * res);
  void handle_configNetTZ(httpsserver::HTTPRequest * req,  httpsserver::HTTPResponse * res);
  void post_configNetTZ(httpsserver::HTTPRequest * req,  httpsserver::HTTPResponse * res);
  void handle_ls(httpsserver::HTTPRequest * req,  httpsserver::HTTPResponse * res);
  void handle_ls2(httpsserver::HTTPRequest * req,  httpsserver::HTTPResponse * res);
  void handle_ls_l(httpsserver::HTTPRequest * req,  httpsserver::HTTPResponse * res);
  void handle_status(httpsserver::HTTPRequest * req,  httpsserver::HTTPResponse * res);
  void handle_NotFound(httpsserver::HTTPRequest * req, httpsserver::HTTPResponse * res);
  void iotfw_authentication(httpsserver::HTTPRequest * req, httpsserver::HTTPResponse * res, std::function<void()> next);
  void handle_logout(httpsserver::HTTPRequest * req, httpsserver::HTTPResponse * res);
  void hash64pw(String user, String pass, unsigned char *encpw);
  void post_acuser(httpsserver::HTTPRequest * req,  httpsserver::HTTPResponse * res);
  char *trim(char *str);

#define CONTENT_LENGTH_UNKNOWN -1
#define CONTENT_LENGTH_NOT_SET  0
#else
  #include <WebServer.h>
  void handleFileUpload(void);
  void handle_configNetTZ(void);
  void post_configNetTZ(void);
  void handle_ls(void);
  void handle_NotFound(void);
#endif
#include <DNSServer.h>
#include <SPI.h>
#include <FS.h>
#include <LittleFS.h>
#include <WiFiUdp.h>
#include <TimeLib.h>
#include <HttpsOTAUpdate.h>
#include <ESPmDNS.h>
#include <CronAlarms.h>
#include <unistd.h>
#include <string.h>
#include "mbedtls/md.h"
#include "mbedtls/base64.h"
// Does not work too heavy and too many conflicts ie file system and time #include <ESP_Mail_Client.h>
// I will need to create a mini mail sender
typedef struct {
  char ssid[32]="";
  char psk[32]="";
  char hostname[32]="";
  char updateURI[80]="";
  char updateKey[16]="";
  char smtp_host[20]="smtp.gmail.com";
  int smtp_port = 465;
  char from_email[40]="";
  char email_password[40]="";
  char user_domain[40]="";
  char to_email[80]=""; // allow for multiple recipients
} netdata_t;

typedef struct {
  char tz[8] = "";
  int dst = 0;
} tzData_t;

typedef struct {
  netdata_t net;
  tzData_t tz;
} netTzData_t;


void handle_IOTFWroot(void);
void handle_indirectRoot(void);
void handle_ApRoot(void);
bool handleFileRead(String path);
String genNetPicker(void);
String genTZPicker(void);
String genStatus(void);
void reboot(void);
char * ascTimeStatus(void);
char * ascNetState(void);
String getContentType(String filename);
void update(void);
void HttpEvent(HttpEvent_t *event);
String listDir(fs::FS &fs, const char * dirname, uint8_t levels);
String octet(int a);
String humanReadableSize(const size_t bytes);
String varReplace(const String& var);
String lsHTML(void);
void sendTok(char* str);
String StringTok(char* str);
void saveCert(httpsserver::SSLCert * cert);
void printSmtpLines(int ms, char * exp, int timeout);
void sendSMS(String subject, String shortMessage);
void post_emailtest(httpsserver::HTTPRequest * req, httpsserver::HTTPResponse * res);
class iotfw {
 public:
  unsigned int localPort = 2390;      // local port to listen for UDP packets
  IPAddress timeServerIP; // time.nist.gov NTP server address
  WiFiUDP udp;
  void begin();
  void beginAp();
  void beginClient();
  time_t getNtpTime();
  void wDelay(int ms);
  int wifiScan();
  time_t loc2gm(time_t itime);
  time_t gm2loc(time_t itime);
  void run(void);
  void send(const String& str);
  void setUsrTokHandler(usrTokFunction handler);
  CronID_t runIn(int val, int units, OnTick_t handler); 
  static const int sec = 1;
  static const int min = 60;
  static const int hour = min * 60;
  static const int day = hour * 24;
  static const int week = day * 7;
  static const int month = 12;
  #ifdef USESECURE
  void setRoot(THandlerFunction handler);
  //void setContentLength(int len);
  //void on(const String &uri, THandlerFunction handler);
  //void on(const String &uri, String method, THandlerFunction fn);
  //void on(const String &uri, String method, THandlerFunction fn, THandlerFunction ufn);
  #else
  void setRoot(WebServer::THandlerFunction handler);
  void on(const String &uri, WebServer::THandlerFunction handler);
  #endif
 private:
  enum TimeState {offline, invalid, valid};
  TimeState ts = offline;
  void set_default_netTzData(void);
  void runAp(void);
  void sendNTPpacket(IPAddress& address);
  void registerURIs(void);
};


#endif /* _IOTFW_H_ */
