#include <iotfw.h>
/* constants */
const int NTP_PACKET_SIZE = 48; // NTP time stamp is in the first 48 bytes of the message
netTzData_t netTzData;
String style_js = F("<style>\n\
body {\n\
font-family: Arial,Helvetica,sans-serif;\n\
background: #181818;\n\
color: #EFEFEF;\n\
font-size: 16px;\n\
}\n\
.input-group>label {\n\
display: inline-block;\n\
padding-right: 10px;\n\
min-width: 17%;\n\
}\n\
.input-group input,.input-group select {\n\
flex-grow: 1;\n\
}\n\
.range-max,.range-min {\n\
display: inline-block;\n\
padding: 0 5px\n\
}\n\
.hidden {\n\
display: none\n\
}\n\
.switch {\n\
display: block;\n\
position: relative;\n\
line-height: 22px;\n\
font-size: 16px;\n\
height: 22px;\n\
}\n\
.switch input {\n\
outline: 0;\n\
opacity: 0;\n\
width: 0;\n\
height: 0;\n\
}\n\
.slider {\n\
width: 50px;\n\
height: 22px;\n\
border-radius: 22px;\n\
cursor: pointer;\n\
background-color: grey;\n\
}\n\
.slider,.slider:before {\n\
display: inline-block;\n\
transition: .4s;\n\
}\n\
.slider:before {\n\
position: relative;\n\
content: \"\";\n\
border-radius: 50%;\n\
height: 16px;\n\
width: 16px;\n\
left: 4px;\n\
top: 3px;\n\
background-color: white;\n\
}\n\
input:checked+.slider {\n\
background-color: #ff3034;\n\
}\n\
input:checked+.slider:before {\n\
-webkit-transform: translateX(26px);\n\
transform: translateX(26px);\n\
}\n\
a {\n\
background-color: blue;\n\
color: white;\n\
text-align: center;\n\
display: block;\n\
padding: 0.5em 1.5em;\n\
margin-top: 1em;\n\
margin-bottom: 1em;\n\
line-height: 3.5;\n\
text-decoration: none;\n\
text-transform: none;\n\
}\n\
</style>\n");
String footer_links = F("<a href =\"/\">Root</a><br>\n\
<a href =\"/login\">login</a><br>\n\
<a href =\"/logout\">logout</a><br>\n\
<a href =\"/configNetTZ\">Configure Network and time zone</a><br>\n");
const char netTzHTML[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML>
<html><head>
<title>Setup</title>
%STYLE_JS%
</head>
<body>
<h1>Status</h1>
%STATUS%
<hr>
<h1>Network Settings</h1>
<form action="/postNetTZ" method="post">
%NETPICKER%
<label for="psk">Network Key</label>
<input type=text id="psk" name="psk" value="%PSK%"><b<br>
<label for="hname">host name</label>
<input type=text id="hname" name="hname" value="%HOSTNAME%"><br>
<label for="updateuri">Update URI</label>
<input type=text id="updateuri" name="updateuri" size=80 maxlength=80 value="%UPDATEURI%"><br>
<input type=submit name="update" value=Update>
<label for="updatekey">Update key</label>
<input type=text id="updatekey" name="updatekey" size=16 maxlenght=16 value="%UPDATEKEY%"><br>
%TZPICKER%
<label for="smtp_host">SMTP Host</label>
<input type=text id="smtp_host" name="smtp_host" size=20 maxlenght=20 value="%SMTP_HOST%">
<label for="smtp_host">SMTP Port</label>
<input type=integer id="smtp_port" name="smtp_port"  value="%SMTP_PORT%"><br>
<label for="from_email">From email</label>
<input type=text id="from_email" name="from_email" size=40 maxlenght=40 value="%FROM_EMAIL%"><br>
<label for="email_password">Email password</label>
<input type=text id="email_password" name="email_password" size=40 maxlenght=40 value="%EMAIL_PASSWORD%"><br>
<label for="user_domain">User domain</label>
<input type=text id="user_domain" name="user_domain" size=40 maxlenght=40 value="%USER_DOMAIN%"><br>
<label for="to_email">To email</label>
<input type=text id="to_email" name="to_email" size=40 maxlenght=80 value="%TO_EMAIL%"><br>
<input type=submit name="cancel" value=Cancel>
<input type=submit name="save" value=Save>
<input type=submit name="reboot" value=Reboot>
</form>
<p>Click the 'Save' button to set the network parameters and reboot.</p>
<hr>
<h1>File Upload</h1>
<p>Free Storage: %FREELittleFS% | Used Storage: %USEDLittleFS% | Total Storage: %TOTALLittleFS%</p>
<form method="POST" action="/upload" enctype="multipart/form-data"><input type="file" name="data"/><input type="submit" name="upload" value="Upload" title="Upload File"></form>
<p>After clicking upload it will upload and be written to LittleFS.</p>
%FILELIST%
<hr>
<h1>Change or add password</h1>
 <form action="/postacuser" method="post">
 <label for="user">User Name</label><input %DISABLE% type="text" maxlength=32 id="user" name="user" value="%USER%"><br>
 <label for="password">Password</label><input type="password" maxlength=64 id="password" name="password" value=""><br>
 <label for="cpassword">Confirm password</label><input type="password" maxlength=64 id="cpassword" name="cpassword" value=""><br>
 <input type=submit name="submitpw" value="Add or Change Password">
 </form>
<hr>
<h1>Emai / SMS testing</h1>
 <form action="/postEmailTest" method="post">
 <label for="subject">Subject</label><input type="text" maxlength=32 id="subject" name="subject" value=""><br>
 <label for="message">Test Message</label><input type="text" maxlength=32 id="message" name="message" value=""><br>
 <input type=submit name="send" value="Send">
 </form>
<hr>
%FOOTER_LINKS%
</body>
</html>
)rawliteral";
const char statusHTML[] PROGMEM = R"rawliteral(
Network state %NETSTATE%<br>
Hostname %HOSTNAME%<br>
IP %LOCALIP%<br>
Mask %SUBNETMASK%<br>
Gateway %GATEWAYIP%<br>
DNS %DNS%<br>
SSID %SSID%<br>
update %UPDATEURI%<br>
current time %TIME% %TIMEZONE% %DST% %TIMESTATUS%<br>
)rawliteral";
const char loginHTML[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML>
<html><head>
<title>Login</title>
%STYLE_JS%
<script>
function login(){
  var url= "https://"+window.location.hostname;
  window.location=url
}
</script>
</head>
<body onload="login();">
<h1>Log in</h1>
Your are being redirected to the HTTPS port<br>
</body>
</html>
)rawliteral";

const char logoutHTML[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML>
<html><head>
<title>Logout</title>
%STYLE_JS%
<script>
function logout() {
  if (bowser.msie) {
    document.execCommand('ClearAuthenticationCache', 'false');
  } else if (bowser.gecko) {
    $.ajax({
      async: false,
      url: '/logout',
      type: 'GET',
      username: 'logout'
      });
  } else if (bowser.webkit) {
    var xmlhttp = new XMLHttpRequest();
    xmlhttp.open("GET", "/logout", true);
    xmlhttp.setRequestHeader("Authorization", "Basic logout");
    xmlhttp.send();
  } else {
      alert("Logging out automatically is unsupported for " + bowser.name
      + "\nYou must close the browser to log out.");
  }
}
</script>
</head>
<body onload="logout();">
<h1>Logged out</h1>
Your are now loged out of %HOSTNAME%<br>
current time %TIME% %TIMEZONE% %DST% %TIMESTATUS%<br>
%FOOTER_LINKS%
</body>
</html>
)rawliteral";

int numNetworks;
time_t seconds;
time_t ltime;
time_t diff_time;
struct tm * timeinfo;
struct tm * gmtt;
const char* ntpServerName = "1.north-america.pool.ntp.org";
byte packetBuffer[ NTP_PACKET_SIZE]; //buffer to hold incoming and outgoing packets
IPAddress apIP(192, 168, 4, 1);
enum NetState {try_ap, ap_ok, ap_error, try_sta, online};
NetState iot_state = try_ap;
static HttpsOTAStatus_t otastatus;
#ifdef USESECURE
httpsserver::SSLCert * cert = new httpsserver::SSLCert();
httpsserver::ResourceParameters *params;
httpsserver::HTTPRequest * __req;
httpsserver::HTTPResponse * __res;
int __contentLength =0;
THandlerFunction roothandler;
#else
const char* www_username = "admin";
const char* www_password = "esp32";
WebServer server(80);
WebServer::THandlerFunction roothandler;
#endif
usrTokFunction usrTokHandler = NULL;
#define DEBUG 1
/* end constants and variables */ 

DNSServer dnsServer;
char gmailCert [2048]; // add code to load it later
WiFiClientSecure smtpClient;
#ifdef USESECURE
 httpsserver::HTTPSServer secureServer =  httpsserver::HTTPSServer(cert);
 httpsserver::HTTPServer server =  httpsserver::HTTPServer();
// Declare some handler functions for the various URLs on the server
// We declare a function that will be the entry-point for the task that is going to be
// created.

void create_cert(void) {
  String ptr = "CN=";
  ptr += netTzData.net.hostname;
  ptr += ",O=jmlzone,C=US";
  int createCertResult = createSelfSignedCert(
			   *cert,
                           httpsserver::KEYSIZE_2048,
                           ptr.c_str(),
                           "20190101000000",
                           "20300101000000"
                         );
  if (createCertResult != 0){
    Serial.print("Creating certificate failed. Error Code = "); Serial.println(createCertResult);
  } else {
    Serial.println(F("Creating the certificate was successful"));
  }
}
void saveCertKey(httpsserver::SSLCert * cert){
  if((cert->getPKLength()>0) && (cert->getCertLength()>0)) {
    File f = LittleFS.open("/cert.dat", "w");
    u_int16_t kl,cl;
        if(f>0) {
      kl = cert->getPKLength();
      cl = cert->getCertLength();
      f.write((const uint8_t * ) &kl, sizeof(u_int16_t));
      f.write((const uint8_t * ) &cl, sizeof(u_int16_t));
      f.write((const uint8_t * ) cert->getPKData(), kl);
      f.write((const uint8_t * ) cert->getCertData(), cl);
      f.close();
      Serial.println(F("Certificate saved."));
    } else {
      Serial.println(F("Unable to open cert.dat for writing."));
    }
  } else {
    Serial.println(F("Will not write a zero lenght certificate."));
  }
}
int loadCertKey(httpsserver::SSLCert * cert){
    File f = LittleFS.open("/cert.dat", "r");
    uint16_t lk,lc, r;
    unsigned char * bk;
    unsigned char * bc;
    r=0;
    if(f>0) {
      r = f.read((uint8_t * ) &lk, sizeof(u_int16_t));
      if(r == sizeof(u_int16_t)) {
	r = f.read((uint8_t * ) &lc, sizeof(u_int16_t));
	if((r == sizeof(u_int16_t)) && (lk>0) && (lc>0)) {
	    bk = new unsigned char[lk];
	    bc = new unsigned char[lc];
	    r=f.read((uint8_t *) bk,lk);
	    if(r==lk) {
	      r=f.read((uint8_t * ) bc,lc);
	      if(r==lc) {
		Serial.println(F("successfully read certificate and key"));
		cert->setPK(bk,lk);
		cert->setCert(bc,lc);
		r=1;
	      }
	    }
	}
      }
      close(f);
    } else {
      Serial.println(F("Unable to open cert.dat for reading."));
    }
    return(r); // 1=success, 0= fail; anything else is a different fail.
}
#endif
void post_emailtest(httpsserver::HTTPRequest * req, httpsserver::HTTPResponse * res) {
  params = req->getParams();
  __req=req;
  __res=res;
  httpsserver::HTTPURLEncodedBodyParser parser(req);
  // if passwords password != cpassword return to form.
  // otherwise add/change password
  char subject[40];  
  char message[80];
  char pvalue[80];
  char * value;
  size_t bl;
  if(DEBUG) {Serial.println(__LINE__);}
  String ptr = "<!DOCTYPE html> <html>\n";
  ptr +="<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0, user-scalable=no\">\n";
  ptr +="<title>Email Test</title>\n%STYLE_JS%";
  ptr +="</head>\n";
  ptr +="<body>\n";
  ptr +="<h1>Email Test</h1>\n";
  if(DEBUG) {Serial.println(__LINE__);}
  while (parser.nextField()) {
    std::string name_o = parser.getFieldName();
    String name = String(name_o.c_str());
    bl=parser.read((byte *)pvalue,80);
    //if(DEBUG) {Serial.println(__LINE__);}
    pvalue[bl] = 0;
    value = trim(pvalue); // trim leading and trailing whitespace
    Serial.println("Name :" + String(bl) + ", " + name + " : " + String(value));
    if(name.substring(0,7).equals("subject")) {
      Serial.println("Subject: " + (String) value);
      strncpy(subject,value,40);}
    if(name.substring(0,7).equals("message")) {
      Serial.println("Message: " + (String) value);
      strncpy(message,value,80);}
  } // while fields
  ptr+="</body></html>";
  if(req->getMethod() != "POST") {
    iotfw_send(405, "text/plain", "Non Post Method");
  } else {
    sendSMS(String(subject), String(message));
    sendTok((char *) ptr.c_str());
    //if(DEBUG) {Serial.println(__LINE__);}
  }
}
void printSmtpLines(int ms, char * exp=NULL, int timeout=0) {
  long currentTime, giveupTime;
  bool gotExp = false;
  int wt;
  char recstr[20];
  int ptr = 0;
  int l;
  if(exp==NULL) {
    gotExp = true;
    wt=ms;
    l=0;
  } else {
    wt = timeout;
    l = strlen(exp);
  }
  currentTime=millis();
  giveupTime=currentTime + wt;
  while (smtpClient.available() || (currentTime < giveupTime)) {
    /* if data is available then receive and print to Terminal */
    if(smtpClient.available()) {
      char c = smtpClient.read();
      Serial.write(c);
      if(!gotExp) {
	      if(ptr<=l) {
	        recstr[ptr]=c;
	        if(ptr==l) {
	          if(!strncmp(exp,recstr,l)) {
	            gotExp = true;
	            Serial.print("Got exp ");
	            Serial.println(exp);
	            wt=ms;
	          }
	        }
	      }
	     if(c=='\n' || c=='\r') {
	       ptr=0;
	     } else if(ptr<19) {ptr++;}
        }
        currentTime=millis();
        giveupTime=currentTime + wt;
      } else {
      delay(10);
        currentTime=millis();
      }
  }
}

void sendSMS(String subject, String shortMessage){
  Serial.println(shortMessage);
  if(iot_state == online) {
    size_t outlen;
    char encVal[80];
    Serial.printf("Connect to %s on %d\n",netTzData.net.smtp_host,netTzData.net.smtp_port);
    if (!smtpClient.connect(netTzData.net.smtp_host,netTzData.net.smtp_port)){
        Serial.println("SMTP Connection failed!");
    } else {
        Serial.println("Connected to server!\n Starting Dialog");
        /* create email dialog */
        Serial.printf("EHLO %s\n",netTzData.net.user_domain);
        smtpClient.printf("EHLO %s\n",netTzData.net.user_domain);
	printSmtpLines(100,"220",1000);
        Serial.println("AUTH LOGIN");
        smtpClient.println("AUTH LOGIN");
        printSmtpLines(100,"334 VXNlcm5hbWU6",1000);
	mbedtls_base64_encode((unsigned char*) encVal, 64, &outlen, (const unsigned char *) netTzData.net.from_email, strlen(netTzData.net.from_email));
	encVal[outlen]=0;
        Serial.printf("my user name '%s' (base 64 encoded)\n",encVal);
        smtpClient.println(String(encVal));
	printSmtpLines(100,"334 UGFzc3dvcmQ6",1000);
	mbedtls_base64_encode((unsigned char *) encVal, 64, &outlen, (const unsigned char *) netTzData.net.email_password, strlen(netTzData.net.email_password));
	encVal[outlen]=0;
        Serial.printf("my password '%s' (base 64 encoded)\n",encVal);
        smtpClient.println(String(encVal));
	printSmtpLines(100,"235",1000); // password OK
	Serial.printf("MAIL FROM: <%s>\n",netTzData.net.from_email);
	smtpClient.printf("MAIL FROM: <%s>\n", netTzData.net.from_email);
	printSmtpLines(100,"250",1000);
        Serial.printf("RCPT TO: <%s><%s>\n",netTzData.net.to_email,netTzData.net.to_email); // yes it should be duplicated like this for gmail
        smtpClient.printf("RCPT TO: <%s><%s>\n",netTzData.net.to_email,netTzData.net.to_email);
	printSmtpLines(100,"250",1000);
        Serial.println("DATA");
        smtpClient.println("DATA");
	printSmtpLines(100);
        Serial.printf("From: %s\n", netTzData.net.from_email); // the From: and To: are not stricly needed
        smtpClient.printf("From: %s\n", netTzData.net.from_email);
	printSmtpLines(100);
        Serial.printf("To: %s\n", netTzData.net.to_email);
        smtpClient.printf("To: %s\n", netTzData.net.to_email);
	printSmtpLines(100);
        Serial.printf("Subject: %s\n\n", subject.c_str()); // need a blank line between the subject and body
        smtpClient.printf("Subject: %s\n\n", subject.c_str());
	printSmtpLines(100);
        Serial.println(shortMessage);
        smtpClient.println(shortMessage);
	printSmtpLines(100);
        Serial.println(".");
        smtpClient.println(".");
	printSmtpLines(100);
        Serial.println("QUIT");
        smtpClient.println("QUIT");
	printSmtpLines(100);

        /* if the server disconnected, stop the smtpClient */
        if (!smtpClient.connected()) {
	  Serial.println();
	  Serial.println("Server disconnected");
	  smtpClient.stop();
        }
    } // else if !connected
  } //if online
}

void iotfw::begin() {
  int r;
  int l=0;
  // Set correct name for dchp?
  // https://techoverflow.net/2021/01/29/how-to-set-wifi-dhcp-hostname-on-esp32-esp8266-different-from-espressif/
  //
  // STA, Config, Hostname sequence in:
  // https://randomnerdtutorials.com/esp32-set-custom-hostname-arduino/
  //WiFi.config(INADDR_NONE, INADDR_NONE, INADDR_NONE);
  WiFi.setHostname(netTzData.net.hostname);
  WiFi.hostname(netTzData.net.hostname);
  if(!LittleFS.begin(true)) { // true will make spiffs it it does not exist
    Serial.println(F("No LittleFS, will be an AP"));
    iot_state = try_ap;
  } else {
    File f = LittleFS.open("/net_tz.dat", "r");
    Serial.print("Net Open Returned: ");
    Serial.println(f);

    if(!f) { // file does not exist
      set_default_netTzData();
      Serial.println(F("No network file, will be an AP"));
      iot_state = try_ap;
    } else {
      r=f.read((byte*)&netTzData,sizeof(netTzData));
      Serial.print("Read "); Serial.print(r); Serial.print(F(" Bytes position is now: ")); Serial.println(f.position());
      f.close();
      if(r==sizeof(netTzData)){ 
	setenv("TZ", netTzData.tz.tz, 1);
	tzset();
	iot_state=try_sta;
      } else {
	set_default_netTzData();
	Serial.println("Bad network file, will be an AP");
	iot_state = try_ap;
      }
    }
    #ifdef USESECURE
    r=loadCertKey(cert);
    Serial.print(F("Certificate load returned: ")); Serial.println(String(r));
    if(r!=1) { // did not load successfully
      Serial.println(F("No certificate file creating one"));
      create_cert();
      saveCertKey(cert);
    } else {
      Serial.println("Cert OK");
    }
    #endif
    // load cert for gmail session
    size_t outlen;
    f = LittleFS.open("/gmailCert.txt", "r");
    outlen = f.available();
    outlen = f.read((uint8_t *) gmailCert,outlen);
    gmailCert[outlen] =0;
    f.close();
    Serial.printf("Read gmail cert %d bytes.\n",outlen);
    /* set SSL/TLS certificate */
    smtpClient.setCACert(gmailCert);
  } // have a LittleFS
  // Set WiFi to station mode and disconnect from an AP if it was previously connected
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  r = wifiScan();
  //if(DEBUG) {Serial.println(__LINE__);}
  if((r!=-1) && (iot_state==try_sta)) {
    WiFi.disconnect();
    WiFi.scanDelete();
    WiFi.mode(WIFI_STA);
    WiFi.hostname(netTzData.net.hostname);
    WiFi.setHostname(netTzData.net.hostname);
    WiFi.begin(netTzData.net.ssid, netTzData.net.psk);
    WiFi.hostname(netTzData.net.hostname);
    WiFi.setHostname(netTzData.net.hostname);
    r=10;
    if(DEBUG) {Serial.println(String(__LINE__) + " Waiting for Wifi Connection." );}
    while ((WiFi.status() != WL_CONNECTED) && (r > -1)) {
     delay(1000);
     if(DEBUG) {Serial.println(String(__LINE__) + " " + String(r));}
     r=r-1;
    }
  }
  if((r==-1)||(WiFi.status() != WL_CONNECTED)) {
    iot_state = try_ap;
  } else {
    r=5;
    udp.begin(localPort);
    while(! seconds && r>0) {
      seconds = getNtpTime();
      if (seconds) {
	setTime(seconds);
	timeinfo = localtime (&seconds);
      }
	r--;
    }
    if(seconds) {
      gmtt = gmtime(&seconds);
      ltime = mktime(timeinfo);
      diff_time = seconds - ltime;
      // LittleFS.setTimeCallback(now());
    }
    //if(DEBUG) {Serial.println(__LINE__);}
  }
  if(iot_state==try_ap) {
    beginAp();
  } else {
    beginClient();
  }
  #ifdef USESECURE
  // We pass:
  // serverTask - the function that should be run as separate task
  // "https443" - a name for the task (mainly used for logging)
  // 6144       - stack size in byte. If you want up to four clients, you should
  //              not go below 6kB. If your stack is too small, you will encounter
  //              Panic and stack canary exceptions, usually during the call to
  //              SSL_accept.
  secureServer.start();
  server.start();
  #endif
  Serial.println("Startup Status");
  Serial.println(StringTok((char *) statusHTML));
}
void iotfw::wDelay(int ms) {
  for(int w = 0; w<ms; w=w+10) {
    seconds = now();
    timeinfo = localtime (&seconds);
    this->run();
    Cron.delay(10);
  }
}
#ifdef USESECURE
void handle_configNetTZ(httpsserver::HTTPRequest * req, httpsserver::HTTPResponse * res) {
  params = req->getParams();
  __req=req;
  __res=res;
  sendTok((char*) netTzHTML);
}
void handle_status(httpsserver::HTTPRequest * req, httpsserver::HTTPResponse * res) {
  params = req->getParams();
  __req=req;
  __res=res;
  sendTok((char*) statusHTML);
}
void handle_NotFound(httpsserver::HTTPRequest * req, httpsserver::HTTPResponse * res) {
  params = req->getParams();
  __req=req;
  __res=res;
  //if(DEBUG) {Serial.println("enter handle_NotFound");}
  if (!handleFileRead(req->getRequestString().c_str())) // send it if it exists
     iotfw_send(404, "text/plain", "Not found");        // othewise error
}
void handle_ApRoot() {
  //if(DEBUG) {Serial.println("enter handle_ApRoot");}
  sendTok((char*) netTzHTML);
}
bool handleFileRead(String path) { // send the right file to the client (if it exists)
  //if(DEBUG) {Serial.println("enter handle_FileRead");}
  //Serial.println("handleFileRead: " + path);
  if (path.endsWith("/")) path += "index.html";         // If a folder is requested, send the index file
  //if(DEBUG) {Serial.println(__LINE__);}
  String contentType = getContentType(path);            // Get the MIME type
  //if(DEBUG) {Serial.println(__LINE__);}
  if (LittleFS.exists(path)) {                            // If the file exists
    File f = LittleFS.open(path.c_str(), "rb");        // Open it
    //if(DEBUG) {Serial.println(__LINE__);}
    if(DEBUG) {Serial.print("Sending file: "); Serial.print(path); Serial.print(", Handle: "); Serial.print(f); Serial.print(", type: "); Serial.println(contentType);}
    //size_t sb = server.streamFile(file, contentType); // And send it to the client
    //server.setContentLength(CONTENT_LENGTH_UNKNOWN);
    setContentLength(f.available());
    iotfw_send(200,contentType,"");
    uint8_t * buf = (uint8_t *)malloc(1360);
    if(!buf){
        return 0;
    }
    int toRead = 0, toWrite = 0, written = 0;
    int available = f.available();
    //Serial.print("available: "); Serial.println(f.available());
    //Serial.print("Position: "); Serial.println(f.position());
    while(available){
        toRead = (available > 1360)?1360:available;
	Serial.print("To Read: "); Serial.println(toRead);
        toWrite = f.read((byte*)buf, toRead);
	//Serial.print("To write: "); Serial.println(toWrite);
	//Serial.print("available: "); Serial.println(f.available());
	//Serial.print("Position: "); Serial.println(f.position());
        written += toWrite;
	iotfw_sendContent((const uint8_t *)buf, toWrite);
	//Serial.print("Written: "); Serial.println(written);
	if(toRead<1360) {
	  available = 0;
	} else {
	  available = f.available();
	}
    }
    free(buf);
    setContentLength(CONTENT_LENGTH_NOT_SET);
    if(DEBUG) {Serial.print("Sent: "); Serial.println(written);}
    //if(DEBUG) {Serial.println(__LINE__);}
    f.close();                                       // Then close the file again
    //if(DEBUG) {Serial.println(__LINE__);}
    return true;
  }
  Serial.println("\tFile Not Found");
  return false;                                         // If the file doesn't exist, return false
}
void iotfw_authentication(httpsserver::HTTPRequest * req, httpsserver::HTTPResponse * res, std::function<void()> next) {
  std::string reqUsername = req->getBasicAuthUser();
  std::string reqPassword = req->getBasicAuthPassword();
  bool authValid = false;
  unsigned char encpw[45];
  if (req->getRequestString() == "/logout") { next();
    Serial.println("Access bypass for /logout");
  } // allow /logout without a login
  // If the user entered login information, we will check it
  if (reqUsername.length() > 0 && reqPassword.length() > 0){
    hash64pw(reqUsername.c_str(), reqPassword.c_str(), encpw);
    if(LittleFS.exists("/passwd")) { // file exists
      File f = LittleFS.open("/passwd","r");
      size_t len, read;
      char line[64];
      char * pwuser;
      //if(DEBUG) {Serial.print(__LINE__); Serial.println(" Password file opened");}
      //      while((read=getline(&line, &len, f)) != -1) {
      while(read=f.readBytesUntil('\n',line,64)) {
	//if(DEBUG) {Serial.print(__LINE__); Serial.print(" read="); Serial.println(read);}
	line[read] = 0;
	pwuser = strtok(line,":");
	//if(DEBUG) {Serial.print(__LINE__); Serial.print(" Got user: "); Serial.println(pwuser);}
	if(strcmp(pwuser,reqUsername.c_str()) ==0) { // found the user, check the password
	  pwuser = strtok(NULL,":");
	  //if(DEBUG) {Serial.print(__LINE__); Serial.print(" Password length "); Serial.println(strlen(pwuser));}
	  //if(DEBUG) {Serial.print(__LINE__); Serial.print(" Supplied password hash "); Serial.println((const char *) encpw);}
	  //if(DEBUG) {Serial.print(__LINE__); Serial.print("     file password hash "); Serial.println((const char *) pwuser);}
	  if(strcmp((const char *) encpw,pwuser)==0) { // check the password
	    authValid = true;
	    //if(DEBUG) {Serial.print(__LINE__); Serial.println(" matched.");}
	    break;
	  } else { // password did not match
	    //if(DEBUG) {Serial.print(__LINE__); Serial.println(" no match.");}
	    break;
	  }
	} // not this user
      } // in file
      f.close();
    } else if (reqUsername == "admin" && reqPassword == "secret") { // fall back no password file
      if(DEBUG) {Serial.print(__LINE__); Serial.println(" No password file fall back authorization");}
      authValid = true;
    } 
    Serial.print("authorization :");
    Serial.print(reqUsername.c_str());
    //Serial.print(" pass: ");
    //Serial.print(reqPassword.c_str());
    //Serial.print("Encoded password: "); 
    //Serial.println((const char *) encpw);
    if (authValid) {
      Serial.println(" Authorized");
    }else{
      Serial.println(" Not allowed!!");
    }
  }
  if (authValid) {
    next();
  } else {
    // Display error page
    res->setStatusCode(401);
    res->setStatusText("Unauthorized");
    res->setHeader("Content-Type", "text/plain");
    String realm = StringTok("Basic realm = \"Secure %HOSTNAME% area\"");
    res->setHeader("WWW-Authenticate", realm.c_str());
        res->println("401. Unauthorized! Contact your System administator for help");
  }
}
void handle_login(httpsserver::HTTPRequest * req, httpsserver::HTTPResponse * res){
  params = req->getParams();
  __req=req;
  __res=res;
  //send
  iotfw_send(200, "text/html", StringTok((char*) loginHTML));
}
void handle_logout(httpsserver::HTTPRequest * req, httpsserver::HTTPResponse * res){
  params = req->getParams();
  __req=req;
  __res=res;
  //send
  iotfw_send(401, "text/html", StringTok((char*) logoutHTML));
}
#else
void handle_configNetTZ() {
  sendTok((char*) netTzHTML);
}
void handle_NotFound(){
  //if(DEBUG) {Serial.println("enter handle_NotFound");}
   if (!handleFileRead(server.uri()))                  // send it if it exists
     server.send(404, "text/plain", "Not found");      // othewise error
}

void handle_ApRoot() {
  //if(DEBUG) {Serial.println("enter handle_ApRoot");}
  sendTok((char*) netTzHTML);
}

bool handleFileRead(String path) { // send the right file to the client (if it exists)
  //if(DEBUG) {Serial.println("enter handle_FileRead");}
  //Serial.println("handleFileRead: " + path);
  if (path.endsWith("/")) path += "index.html";         // If a folder is requested, send the index file
  //if(DEBUG) {Serial.println(__LINE__);}
  String contentType = getContentType(path);            // Get the MIME type
  //if(DEBUG) {Serial.println(__LINE__);}
  if (LittleFS.exists(path)) {                            // If the file exists
    File f = LittleFS.open(path.c_str(), "rb");        // Open it
    //if(DEBUG) {Serial.println(__LINE__);}
    if(DEBUG) {Serial.print("Sending file: "); Serial.print(path); Serial.print(", Handle: "); Serial.print(f); Serial.print(", type: "); Serial.println(contentType);}
    //size_t sb = server.streamFile(file, contentType); // And send it to the client
    //server.setContentLength(CONTENT_LENGTH_UNKNOWN);
    server.setContentLength(f.available());
    server.send(200,contentType,"");
    uint8_t * buf = (uint8_t *)malloc(1360);
    if(!buf){
        return 0;
    }
    int toRead = 0, toWrite = 0, written = 0;
    int available = f.available();
    //Serial.print("available: "); Serial.println(f.available());
    //Serial.print("Position: "); Serial.println(f.position());
    while(available){
        toRead = (available > 1360)?1360:available;
	Serial.print("To Read: "); Serial.println(toRead);
        toWrite = f.read((byte*)buf, toRead);
	//Serial.print("To write: "); Serial.println(toWrite);
	//Serial.print("available: "); Serial.println(f.available());
	//Serial.print("Position: "); Serial.println(f.position());
        written += toWrite;
	server.sendContent((char *)buf, toWrite);
	//Serial.print("Written: "); Serial.println(written);
	if(toRead<1360) {
	  available = 0;
	} else {
	  available = f.available();
	}
    }
    free(buf);
    server.setContentLength(CONTENT_LENGTH_NOT_SET);
    if(DEBUG) {Serial.print("Sent: "); Serial.println(written);}
    //if(DEBUG) {Serial.println(__LINE__);}
    f.close();                                       // Then close the file again
    //if(DEBUG) {Serial.println(__LINE__);}
    return true;
  }
  Serial.println("\tFile Not Found");
  return false;                                         // If the file doesn't exist, return false
}
#endif
void iotfw::beginAp() {
  Serial.print("Configuring access point...");
  /* You can remove the password parameter if you want the AP to be open. */
  WiFi.disconnect(); // cleanup;
  WiFi.mode(WIFI_OFF);
  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(apIP,apIP,IPAddress(255,255,255,0));
  if(WiFi.softAP(netTzData.net.hostname)) {
    Serial.println("Soft AP OK");
    iot_state = ap_ok;
  } else {
    Serial.println("Soft AP startup error");
    iot_state = ap_error;
  }
  dnsServer.start(53, "*", apIP); // start captive AP
  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);
  roothandler = handle_ApRoot;
  iotfw::registerURIs();
  Serial.println("Captive HTTP server started");
}
void iotfw::registerURIs() {
#ifdef USESECURE
  //if(DEBUG) {Serial.println(__LINE__);}
    // Create nodes
  httpsserver::ResourceNode * indetRootRS  = new httpsserver::ResourceNode("/", "GET", &handle_indirectRoot);
  httpsserver::ResourceNode * uploadRS   = new httpsserver::ResourceNode("/upload", "POST", &handleFileUpload);
  httpsserver::ResourceNode * lsRS       = new httpsserver::ResourceNode("/ls", "GET", &handle_ls);
  httpsserver::ResourceNode * ls2RS       = new httpsserver::ResourceNode("/ls2", "GET", &handle_ls2);
  httpsserver::ResourceNode * ls_lRS       = new httpsserver::ResourceNode("/ls_l", "GET", &handle_ls_l);
  httpsserver::ResourceNode * notFoundRS = new httpsserver::ResourceNode("", "GET", &handle_NotFound);
  httpsserver::ResourceNode * netTZRS    = new httpsserver::ResourceNode("/configNetTZ", "GET", &handle_configNetTZ);
  httpsserver::ResourceNode * post_netTZRS = new httpsserver::ResourceNode("/postNetTZ","POST", &post_configNetTZ);
  httpsserver::ResourceNode * statusRS = new httpsserver::ResourceNode("/status","GET", &handle_status);
  httpsserver::ResourceNode * loginRS = new httpsserver::ResourceNode("/login","GET", &handle_login);
  httpsserver::ResourceNode * logoutRS = new httpsserver::ResourceNode("/logout","GET", &handle_logout);
  httpsserver::ResourceNode * post_acuserRS = new httpsserver::ResourceNode("/postacuser","POST", &post_acuser);
  httpsserver::ResourceNode * post_emailRS = new httpsserver::ResourceNode("/postEmailTest","POST", &post_emailtest);

  // Add nodes to the servers
  server.registerNode(indetRootRS);
  server.registerNode(lsRS);
  server.registerNode(ls2RS);
  server.registerNode(statusRS);
  server.registerNode(logoutRS);
  server.registerNode(loginRS);
  server.setDefaultNode(notFoundRS);
  if(iot_state == ap_ok) {
  server.registerNode(netTZRS);
  server.registerNode(post_acuserRS);
  server.registerNode(post_netTZRS);
  }
  secureServer.registerNode(indetRootRS);
  secureServer.registerNode(uploadRS);
  secureServer.registerNode(lsRS);
  secureServer.registerNode(ls2RS);
  secureServer.registerNode(ls_lRS);
  secureServer.registerNode(netTZRS);
  secureServer.registerNode(post_netTZRS);
  secureServer.registerNode(post_acuserRS);
  secureServer.registerNode(post_emailRS);
  secureServer.registerNode(statusRS);
  secureServer.registerNode(loginRS);
  secureServer.registerNode(logoutRS);
  secureServer.setDefaultNode(notFoundRS);
  secureServer.addMiddleware(&iotfw_authentication);
  
  Serial.println("Starting Secure server...");
  secureServer.start();
  server.start();
  if (secureServer.isRunning()){
    Serial.println("Server ready.");
  }
#else
  server.on("/", handle_indirectRoot);
  server.on("/ls", handle_ls);
  server.on("/configNetTZ",handle_configNetTZ);
  server.on("/postNetTZ",post_configNetTZ);
  server.on("/login", []() {
    if (!server.authenticate(www_username, www_password)) {
      return server.requestAuthentication();
    }
    server.send(200, "text/plain", "Login OK");
  });

  //when all else fails
  server.onNotFound(handle_NotFound);
  server.begin();
  Serial.println("Client HTTP server started");
#endif
}
void iotfw::beginClient() {
  IPAddress myIP = WiFi.localIP();
  Serial.print("My IP address: ");
  Serial.println(myIP);
  roothandler = handle_ApRoot;
  if(MDNS.begin(netTzData.net.hostname)) {
    Serial.print("Starting mDNS as ");
    Serial.println(netTzData.net.hostname);
    MDNS.addService("http", "tcp", 80);
    //if(DEBUG) {Serial.println(__LINE__);}
  }else {
    Serial.println("Error starring mDNS");
  }
  //if(DEBUG) {Serial.println(__LINE__);}
  iotfw::registerURIs();
  iot_state = online;
}

void iotfw::run() {
  if(iot_state != online) {
    dnsServer.processNextRequest();
  }
#ifdef USESECURE
  secureServer.loop();
  server.loop();
#else
  server.handleClient();
#endif
}

#ifdef USESECURE
/*
*/
#else
void iotfw::on(const String &uri, WebServer::THandlerFunction handler) {
  server.on(uri, handler);
}
#endif
void reboot() {
  ESP.restart();
}

// send an NTP request to the time server at the given address
//unsigned long iotfw::sendNTPpacket(IPAddress& address)
void iotfw::sendNTPpacket(IPAddress& address)
{
  Serial.println("sending NTP packet...");
  // set all bytes in the buffer to 0
  //if(DEBUG) {Serial.println(__LINE__);}
  //memset(packetBuffer, 0, NTP_PACKET_SIZE);
  //if(DEBUG) {Serial.println(__LINE__);}
  // Initialize values needed to form NTP request
  // (see URL above for details on the packets)
  packetBuffer[0] = 0b11100011;   // LI, Version, Mode
  packetBuffer[1] = 0;     // Stratum, or type of clock
  packetBuffer[2] = 6;     // Polling Interval
  packetBuffer[3] = 0xEC;  // Peer Clock Precision
  // 8 bytes of zero for Root Delay & Root Dispersion
  packetBuffer[12]  = 49;
  packetBuffer[13]  = 0x4E;
  packetBuffer[14]  = 49;
  packetBuffer[15]  = 52;

  // all NTP fields have been given values, now
  // you can send a packet requesting a timestamp:
  //if(DEBUG) {Serial.println(__LINE__);}
  udp.beginPacket(address, 123); //NTP requests are to port 123
  //if(DEBUG) {Serial.println(__LINE__);}
  udp.write(packetBuffer, NTP_PACKET_SIZE);
  //if(DEBUG) {Serial.println(__LINE__);}
  udp.endPacket();
  //if(DEBUG) {Serial.println(__LINE__);}
}

time_t iotfw::getNtpTime(void) {
  WiFi.hostByName(ntpServerName, timeServerIP); 
  sendNTPpacket(timeServerIP); // send an NTP packet to a time server
  //if(DEBUG) {Serial.println(__LINE__);}
  // wait to see if a reply is available
  delay(2000);
  //if(DEBUG) {Serial.println(__LINE__);}
  int cb = udp.parsePacket();
  //if(DEBUG) {Serial.println(__LINE__);}
  if (cb) {
    // We've received a packet, read the data from it
    //if(DEBUG) {Serial.println(__LINE__);}
    udp.read(packetBuffer, NTP_PACKET_SIZE); // read the packet into the buffer
    //if(DEBUG) {Serial.println(__LINE__);}
    //the timestamp starts at byte 40 of the received packet and is four bytes,
    // or two words, long. First, extract the two words:
    unsigned long highWord = word(packetBuffer[40], packetBuffer[41]);
    unsigned long lowWord = word(packetBuffer[42], packetBuffer[43]);
    // combine the four bytes (two words) into a long integer
    // this is NTP time (seconds since Jan 1 1900):
    unsigned long secsSince1900 = highWord << 16 | lowWord;
    // now convert NTP time into everyday time:
    // Unix time starts on Jan 1 1970. In seconds, that's 2208988800:
    const unsigned long seventyYears = 2208988800UL;
    // subtract seventy years:
    unsigned long epoch = secsSince1900 - seventyYears;
    //if(DEBUG) {Serial.println(__LINE__);}
    return(epoch);
  }
  //if(DEBUG) {Serial.println(__LINE__);}
  return(0);
}

char * ascTimeStatus(void) {
  timeStatus_t s = timeStatus();
  if(s==timeNotSet) {
    return("Not Set");
  } else if(s==timeNeedsSync) {
    return("Need Sync");
  } else if(s==timeSet) {
    return("Time OK");
  } else {
    return("No status");
  }
}
char * ascNetState(void) {
  if(iot_state == try_ap) {
    return("try_ap");
  } else if(iot_state == ap_ok) {
    return("AP");
  } else if(iot_state == ap_error) {
    return("AP Error");
  } else if(iot_state == try_sta) {
    return("try_sta");
  } else if(iot_state == online) {
    return("Online");
  } else {
    return("Unknown");
  }
}
#ifdef USESECURE
void iotfw::setRoot(THandlerFunction handler) {
#else
void iotfw::setRoot(WebServer::THandlerFunction handler) {
#endif
  Serial.println("setting new root handler");
  roothandler = handler;
}
 void iotfw::setUsrTokHandler(usrTokFunction handler){
  usrTokHandler = handler;
}
// helper functions for TimeAlarms since they work in GMT and missing access method
time_t iotfw::loc2gm(time_t itime) {
  return( (SECS_PER_DAY + (itime-diff_time))%SECS_PER_DAY);
}

time_t iotfw::gm2loc(time_t itime) {
  return( (SECS_PER_DAY + (itime+diff_time))%SECS_PER_DAY);
}
String genNetPicker() {
   //if(DEBUG) {Serial.println("enter genNetPicker");}
  String ptr="<label for=\"network\">Select Network</label>\n";
  ptr+="<select id=\"network\" name=\"network\">\n";
  if(iot_state == online) {
    ptr+="<option value=\"";
    ptr+=netTzData.net.ssid;
    ptr+="\">";
    ptr+=netTzData.net.ssid;
    ptr+="</option>\n";
  } else if (numNetworks == 0) {
    ptr+="<option value=\"None\">None</option>\n";
  } else {
    for (int i = 0; i < numNetworks; ++i) {
      // Print SSID and RSSI for each network found
      ptr+="<option value=\"";
      ptr+=WiFi.SSID(i);
      ptr+="\">";
      ptr+=WiFi.SSID(i);
      ptr+="</option>\n";
    }
  }
  ptr+="</select><br><br>\n";
  return ptr;
 }

String genTZPicker() {
  //if(DEBUG) {Serial.println("enter genTZPicker");}
  String ptr="<label for=\"tz\">Timezone</label>\n\
<select id=\"tz\" name=\"tz\">\n\
<option value=\"EDT4\">Eastern Daylight</option>\n\
<option value=\"EST5\">Eastern Standard</option>\n\
<option value=\"CST6CDT\">Central</option>\n\
<option value=\"MST7MDT\">Mountain</option>\n\
<option value=\"PST8PDT\">Pacific</option>\n\
</select>";
 ptr +="<input type=\"checkbox\" name=\"dst\" value=\"enabled\" ";
 if(netTzData.tz.dst) {
   ptr += "checked";
 }
 ptr += "> DST";
 ptr +="<br />\n";
 //if(DEBUG) {Serial.println(__LINE__);}
 return ptr;
}

String genStatus(){
  return(StringTok((char *) statusHTML));
}
#ifdef USESECURE
char *trim(char *str){
  size_t len = 0;
  char *frontp = str;
  char *endp = NULL;
  if( str == NULL ) { return NULL; }
  if( str[0] == '\0' ) { return str; }
  len = strlen(str);
  endp = str + len;

  /* Move the front and back pointers to address the first non-whitespace
   * characters from each end.
   */
  while( isspace((unsigned char) *frontp) ) { ++frontp; }
  if( endp != frontp ){
    while( isspace((unsigned char) *(--endp)) && endp != frontp ) {}
  }

  if( frontp != str && endp == frontp )
    *str = '\0';
  else if( str + len - 1 != endp )
    *(endp + 1) = '\0';

  /* Shift the string so that it starts at str so that if it's dynamically
   * allocated, we can still free it on the returned pointer.  Note the reuse
   * of endp to mean the front of the string buffer now.
   */
  endp = str;
  if( frontp != str ) {
    while( *frontp ) { *endp++ = *frontp++; }
    *endp = '\0';
  }
    return str;
}
void post_acuser(httpsserver::HTTPRequest * req, httpsserver::HTTPResponse * res) {
  params = req->getParams();
  __req=req;
  __res=res;
  httpsserver::HTTPURLEncodedBodyParser parser(req);
  // if passwords password != cpassword return to form.
  // otherwise add/change password
  char uvalue[32];  
  char pw[64];  
  char pwc[64];
  char value[80];
  char * tvalue;
  size_t bl;
  String ptr = "<!DOCTYPE html> <html>\n";
  ptr +="<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0, user-scalable=no\">\n";
  ptr +="<title>Added user or changed password</title>\n%STYLE_JS%";
  ptr +="</head>\n";
  ptr +="<body>\n";
  ptr +="<h1>Password Cange</h1>\n";
  strncpy(uvalue,__req->getBasicAuthUser().c_str(),31); // set a default user name if its not specified
  while (parser.nextField()) {
    std::string name_o = parser.getFieldName();
    String name = String(name_o.c_str());
    bl=parser.read((byte *)value,80);
    value[bl] = 0;
    tvalue = trim(value); // trim leading and trailing whitespace
    Serial.println("Name :" + String(bl) + ", " + name + " : " + String(tvalue));
    if(name.substring(0,4).equals("user")) {
      Serial.println("user: " + (String) tvalue);
      strncpy(uvalue,tvalue,31);}
    if(name.substring(0,8).equals("password")) {
      Serial.println("password: " + (String) tvalue);
      strncpy(pw,tvalue,63);}
    if(name.substring(0,9).equals("cpassword")) {
      Serial.println("cpassword: " + (String) tvalue);
      strncpy(pwc,tvalue,63);}
  } // while fields
  if(req->getMethod() != "POST") {
    iotfw_send(405, "text/plain", "Non Post Method");
  } else if(strcmp(pw, pwc) !=0) {
      iotfw_send(200, "text/plain", "Passwords do not match");
  } else { // ok to update the password
    hash64pw(uvalue, pw, (unsigned char *) pwc);
  if(DEBUG) {Serial.print(__LINE__); Serial.print(" password hashed "); Serial.println((const char *) pwc);}
  // add or change a password
  File f = LittleFS.open("/passwd","r");
  File fo = LittleFS.open("/passwd.tmp","w");
  if(fo==NULL) {
    Serial.println("Could not open temp file for output\n");
    iotfw_send(200, "text/plain", "Error opening password file for write");
    return;
  }
  bool pwWritten = false;
  if(f!=NULL) { // existing password file
    size_t len, read;
    char line[64];
    char * line2=NULL;
    char * pwuser;
    while(read=f.readBytesUntil('\n',line,64)) {
      line[read] = 0;
      //Serial.print("got ");Serial.print(read);Serial.print(" chars: ");Serial.println(line);
      line2 = strdup(line);
      pwuser = strtok(line,":");
      //Serial.print("Looking at user "); Serial.println(pwuser);
      if(strcmp(pwuser,uvalue) == 0) { // found the user
	//Serial.print("removing and replacing line ");Serial.println(line);
	len=fo.print(String(uvalue) + ":" + String(pwc) + "\n");
	pwWritten = true;
	ptr += "Changed password for user: " + String(uvalue);
      } else {
	//Serial.print("Writing line :");Serial.println(line2);
	len=fo.print(String(line2) + "\n");
      }
      free(line2);
      read = 0;
    } // while
    f.close();
    LittleFS.remove("/passwd");
    Serial.println("f existed, removed");
  } // f existed
  if(!pwWritten) {
    //Serial.println("adding line :" + String(uvalue) +":"+String(pwc));
    ptr += "Added user: " + String(uvalue);
    bl=fo.print(String(uvalue) + ":" + String(pwc)+"\n");
  }
  ptr +="%FOOTER_LINKS%</body>\n</html>\n";
  sendTok((char * ) ptr.c_str());
  fo.close();
  LittleFS.rename("/passwd.tmp","/passwd");
  }
}
void post_configNetTZ(httpsserver::HTTPRequest * req, httpsserver::HTTPResponse * res) {
  params = req->getParams();
  __req=req;
  __res=res;
  httpsserver::HTTPURLEncodedBodyParser parser(req);
  netTzData_t tmpData;
  bool b_cancel = false;
  bool b_update = false;
  bool b_save = false;
  bool b_reboot = false;
  tmpData.tz.dst=0;
  char pvalue[80]; // preliminary value
  char * value;
  size_t bl;
  while (parser.nextField()) {
    std::string name_o = parser.getFieldName();
    String name = String(name_o.c_str());
    bl=parser.read((byte *)pvalue,80);
    pvalue[bl] = 0;
    value = trim(pvalue);
    Serial.println("Name :" + String(bl) + ", " + name + " : " + String(value));
    if(name.substring(0,7).equals("network")) {
      Serial.println("network: " + (String) value);
      strncpy(tmpData.net.ssid,value,31);}
    if(name.substring(0,3).equals("psk")) {
      Serial.println("    psk: " + (String) value);
      strncpy(tmpData.net.psk,value,31);}
    if(name.substring(0,5).equals("hname")) {
      Serial.println("  hname: " + (String) value);
      strncpy(tmpData.net.hostname, value,31);}
    if(name.substring(0,2).equals("tz")) {
      Serial.println("    tz: " + (String) value);
      strncpy(tmpData.tz.tz, value,8);}
    if(name.substring(0,3).equals("dst")) {
      Serial.println("    dst: " + (String) value);
      if(value=="true") {tmpData.tz.dst=1;}}
    if(name.substring(0,9).equals("updateuri")) {
      Serial.println("updateuri: " + (String) value);
      strncpy(tmpData.net.updateURI, value,80);}
    if(name.substring(0,9).equals("updatekey")) {
      Serial.println("updatekey: " + (String) value);
      strncpy(tmpData.net.updateKey, value,8);}
    if(name.substring(0,9).equals("smtp_host")) {
      Serial.println("SMTP host: " + (String) value);
      strncpy(tmpData.net.smtp_host, value,20);}
    if(name.substring(0,9).equals("smtp_port")) {
      Serial.println("SMTP port: " + (String) value);
      tmpData.net.smtp_port=atoi(value);}
    if(name.substring(0,10).equals("from_email")) {
      Serial.println("From email: " + (String) value);
      strncpy(tmpData.net.from_email, value,40);}
    if(name.substring(0,14).equals("email_password")) {
      Serial.println("email password: " + (String) value);
      strncpy(tmpData.net.email_password, value,40);}
    if(name.substring(0,11).equals("user_domain")) {
      Serial.println("user_domain: " + (String) value);
      strncpy(tmpData.net.user_domain, value,40);}
    if(name.substring(0,9).equals("to_email")) {
      Serial.println("To email: " + (String) value);
      strncpy(tmpData.net.to_email, value,80);}
    if(name.substring(0,6).equals("cancel")) {b_cancel=true; Serial.println("cancel set");}
    if(name.equals("update")) {b_update=true; Serial.println("update set");}
    if(name.substring(0,4).equals("save")) {b_save=true; Serial.println("save set");}
    if(name.substring(0,6).equals("reboot")) {b_reboot=true; Serial.println("reboot set");}
  }
   if(b_cancel)
#else
void post_configNetTZ() {
  if (server.hasArg("cancel"))
#endif
    {
    roothandler();
  } else
#ifdef USESECURE
    if (req->getMethod() != "POST")
#else
    if (server.method() != HTTP_POST)
#endif
      {
      Serial.println("Non post method");
    String message = "Non post POST method: ";
#ifdef USESECURE
    //WIP message += (String) req->getMethod();
    message += "\n";
   iotfw_send(405, "text/plain", message);
#else
    message += String(server.method()) +"\n";
    server.send(405, "text/plain", message);
#endif
  } else {
      Serial.println("Post method");
    String ptr = "<!DOCTYPE html> <html>\n";
    ptr +="<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0, user-scalable=no\">\n";
    ptr +="<title>Set WiFi and time zone</title>\n";
    ptr+=style_js;
    ptr +="</head>\n";
    ptr +="<body>\n";
    ptr +="<h1>Set WiFi</h1>\n";
    ptr +="<h3> server args</h3>";
    #ifdef USESECURE
    //memcpy((char *) &netTzData,(char *) &tmpData,sizeof(netTzData_t));
    netTzData = tmpData;
    ptr += genStatus();
#else
    for (uint8_t i = 0; i < server.args(); i++) {
      ptr += " (" + String(i) +") "+ server.argName(i) + ": " + server.arg(i) + " -- <br>";
      if(server.argName(i).substring(0,7).equals("network")) {
        strncpy(netTzData.net.ssid,server.arg(i).c_str(),31);
      }
      if(server.argName(i).substring(0,3).equals("psk")) {
        strncpy(netTzData.net.psk,server.arg(i).c_str(),31);
      }
      if(server.argName(i).substring(0,5).equals("hname")) {
        strncpy(netTzData.net.hostname,server.arg(i).c_str(),31);
      }
      if(server.argName(i).substring(0,2).equals("tz")) {
        strncpy(netTzData.tz.tz,server.arg(i).c_str(),8);
      }
      if(server.argName(i).substring(0,3).equals("dst")) {
        if(server.arg(i).substring(0,4).equals("true")) {
	  netTzData.tz.dst=1;
	}
      }
      if(server.argName(i).substring(0,9).equals("updateuri")) {
        strncpy(netTzData.net.updateURI,server.arg(i).c_str(),80);
      }
      if(server.argName(i).substring(0,9).equals("updatekey")) {
        strncpy(netTzData.net.updateKey,server.arg(i).c_str(),8);
      }
    }
    #endif
    if((netTzData.net.hostname!="") &&
      #ifdef USESECURE
       b_save
      #else
       server.hasArg("save")
      #endif
       ) {
      ptr +="<br> Saving<br>\n";
      File f = LittleFS.open("/net_tz.dat", "w");
      if(f && f.write((byte*)&netTzData,sizeof(netTzData)) ) {
        f.close();
	ptr +="<br> Success<br>\n";
	ptr +="<br> Rebooting in 10 seconds<br>\n";
      }
    }
    if(
      #ifdef USESECURE
       b_update
      #else
       server.hasArg("update")
      #endif
       ) {
      ptr +="<br>Will update...<br>\n";
      #ifdef USESECURE
      setContentLength(CONTENT_LENGTH_UNKNOWN);
      iotfw_send(200, "text/html", ptr);
      #else
      server.setContentLength(CONTENT_LENGTH_UNKNOWN);
      server.send(200, "text/html", ptr);
      #endif
      update();
    } else {
      ptr +="Not updating...<br>";
    //ptr +=footer_links;
      ptr +="</body>\n";
      ptr +="</html>\n";
      #ifdef USESECURE
      iotfw_send(200, "text/html", ptr);
      #else
      server.send(200, "text/html", ptr);
      #endif
    }
    if(
      #ifdef USESECURE
       b_save || b_reboot
      #else
       server.hasArg("save")
      #endif
      ) {
      delay(10000);
      reboot();
    }
  }
}
   //}
void iotfw::send(const String& str) {
  iotfw_send(200, "text/html", str);
}
//void iotfw::
void iotfw_send(int code, const String & type, const String& str) {
   __res->setStatusCode(code);
   __res->setHeader("Content-Type", type.c_str()); //type);
   if(__contentLength != CONTENT_LENGTH_UNKNOWN) { // tbd not specified, specidifed or get length
     if(__contentLength == CONTENT_LENGTH_NOT_SET) {
       __contentLength = str.length();
     }
     __res->setHeader("Content-Length", httpsserver::intToString(__contentLength));
     __contentLength = CONTENT_LENGTH_NOT_SET;
   }
   __res->print(str);
}
//void iotfw::
void iotfw_sendContent(const String& str) {
   __res->println(str);
}
void iotfw_sendContent(const uint8_t* buf, size_t len) {
   size_t dummy = __res->write(buf, len);
}
// void iotfw::
void setContentLength(int len) {
   __contentLength = len;
}
//send with token (variable) replacement
void sendTok(char* str) {
  String ptr="";
  String key="";
  char c;
  int l,p;
  bool first, inkey;
  first = true;
  inkey = false;
  l = strlen(str);
  #ifdef USESECURE
  setContentLength(CONTENT_LENGTH_UNKNOWN);
  #else
  server.setContentLength(CONTENT_LENGTH_UNKNOWN);
  #endif
  for(int i=0; i<l; i++) {
    c = char(str[i]);
    if(c == '%') {
      if(inkey) { // end of key
	#ifdef USESECURE
	iotfw_sendContent(varReplace(key));
        #else
	server.sendContent(varReplace(key));
	#endif
	key = "";
	inkey = false;
      } else { // start of key
	inkey=true;
	if(first) {
          #ifdef USESECURE
	  iotfw_send(200, "text/html", ptr);   // send initial chunk
          #else
	  server.send(200, "text/html", ptr);   // send initial chunk
	  #endif
	  first = false;
	} else {
	  #ifdef USESECURE
	  iotfw_sendContent(ptr);
	  #else
	  server.sendContent(ptr);
	  #endif
	}
	ptr="";
      }
    } else { //not a %
      if(inkey) {
	key += c;
      } else {
	ptr += c;
      }
    }
  }
  #ifdef USESECURE
  // send final chunk
  iotfw_sendContent(ptr);
  // release lenght override
  setContentLength(CONTENT_LENGTH_NOT_SET);
  #else
  // send final chunk
  server.sendContent(ptr);
  // release lenght override
  server.setContentLength(CONTENT_LENGTH_NOT_SET);
  #endif
}
 String StringTok(char* str){
  String ptr="";
  String key="";
  char c;
  int l,p;
  bool first, inkey;
  first = true;
  inkey = false;
  l = strlen(str);
   for(int i=0; i<l; i++) {
    c = char(str[i]);
    if(c == '%') {
      if(inkey) { // end of key
	ptr+=varReplace(key);
	key = "";
	inkey = false;
      } else { // start of key
	inkey=true;
	if(first) {
	  first = false;
	}
      }
    } else { //not a %
      if(inkey) {
	key += c;
      } else {
	ptr += c;
      }
    }
  }
 return(ptr);
}
int iotfw::wifiScan() {
   Serial.println("scan start");
  int r =-1;
  // WiFi.scanNetworks will return the number of networks found
  numNetworks = WiFi.scanNetworks();
  Serial.println("scan done");
  if (numNetworks == 0) {
    Serial.println("no networks found");
  } else {
    Serial.print(numNetworks);
    Serial.println(" networks found");
    for (int i = 0; i < numNetworks; ++i) {
      // Print SSID and RSSI for each network found
      Serial.print(i + 1);
      Serial.print(": ");
      Serial.print(WiFi.SSID(i));
      Serial.print(" (");
      Serial.print(WiFi.RSSI(i));
      Serial.print(")");
      Serial.println((WiFi.encryptionType(i) == 0) ? " " : "*");
      if(strcmp(WiFi.SSID(i).c_str(),netTzData.net.ssid) ==0) {
        Serial.println("this matched my eeprom data");
        r=i;
      }
    }
  }
  Serial.println("");
  return(r);
}

String getContentType(String filename) { // convert the file extension to the MIME type
  if (filename.endsWith(".html")) return "text/html";
  else if (filename.endsWith(".css")) return "text/css";
  else if (filename.endsWith(".js")) return "application/javascript";
  else if (filename.endsWith(".ico")) return "image/x-icon";
  return "text/plain";
}

void iotfw::set_default_netTzData() {
#ifdef ESP8266
  int chip_id = ESP.getChipId();
#elif defined(ESP32)
  int chip_id = ESP.getEfuseMac();
#endif
    netTzData.net.ssid[0]=0;
    netTzData.net.psk[0]=0;
    netTzData.net.updateURI[0]=0;
    netTzData.net.updateKey[0]=0;
    strcpy(netTzData.net.smtp_host,"smtp.gmail.com");
    netTzData.net.smtp_port = 465;
    strcpy(netTzData.net.user_domain,"mydomain.net");
    netTzData.net.from_email[0] = 0;
    netTzData.net.email_password[0] = 0;
    netTzData.net.to_email[0] = 0;
    String ptr = "iotfw_";
    ptr += String(chip_id,HEX);
    //strncpy(netTzData.net.hostname,ptr,31);
    ptr.toCharArray(netTzData.net.hostname,ptr.length());
    netTzData.net.hostname[31] = 0;
    netTzData.tz.dst=0;
    strncpy(netTzData.tz.tz,"EST5",4);
    netTzData.tz.tz[4] = 0;
}
void update () {
  if(iot_state != online ) {
    Serial.println("not on line can not update");
    #ifdef USESECURE
    iotfw_sendContent("not on line can not update<br>\n");
    #else
    server.sendContent("not on line can not update<br>\n");
    #endif
  } else {
    HttpsOTA.onHttpEvent(HttpEvent);
    Serial.println("Starting OTA");
    #ifdef USESECURE
    iotfw_sendContent("Starting OTA<br>\n");
    #else
    server.sendContent("Starting OTA<br>\n");
    #endif
    HttpsOTA.begin(netTzData.net.updateURI, "", false);
    otastatus = HttpsOTA.status();
    Serial.print("Status=");
    Serial.println(otastatus);
    #ifdef USESECURE
    iotfw_sendContent("...<br>\n");
    #else
    server.sendContent("...<br>\n");
    #endif
    while((otastatus != HTTPS_OTA_SUCCESS) && (otastatus != HTTPS_OTA_FAIL) && (otastatus != HTTPS_OTA_ERR)) {
      otastatus = HttpsOTA.status();
      Serial.print("Status=");
      #ifdef USESECURE
      iotfw_sendContent("Updating...<br>\n");
      #else
      server.sendContent("Updating...<br>\n");
      #endif
      Serial.println(otastatus);
      if(otastatus == HTTPS_OTA_SUCCESS) { 
        Serial.println("Firmware written successfully. Rebooting in 10 seconds");
	#ifdef USESECURE
	iotfw_sendContent("Firmware written successfully. Rebooting in 10 seconds<br>\n");
	#else
	server.sendContent("Firmware written successfully. Rebooting in 10 seconds<br>\n");
	#endif
	for(int d=10; d>0; d--) {
	  #ifdef USESECURE
	  iotfw_sendContent(String(d));
	  iotfw_sendContent(".<br>");
	  #else
	  server.sendContent(String(d));
	  server.sendContent(".<br>");
	  #endif
	  delay(1000);
	}
	#ifdef USESECURE
	iotfw_sendContent("Click <a href=\"/\">here</a> to go back.</body></html>\n");
	#else
	server.sendContent("Click <a href=\"/\">here</a> to go back.</body></html>\n");
	#endif
	delay(1000);
	reboot();
      } else if((otastatus == HTTPS_OTA_FAIL) || (otastatus == HTTPS_OTA_ERR)) { 
        Serial.println("Firmware Upgrade Fail");
	#ifdef USESECURE
	iotfw_sendContent("Firmware Upgrade Fail<br>\n");
	iotfw_sendContent("</body></html>\n");
	#else
	server.sendContent("Firmware Upgrade Fail<br>\n");
	server.sendContent("</body></html>\n");
	#endif
      }
      delay(1000);
    }
  }
}
void HttpEvent(HttpEvent_t *event) {
  switch(event->event_id) {
  case HTTP_EVENT_ERROR:
    Serial.println("Http Event Error");
    break;
  case HTTP_EVENT_ON_CONNECTED:
    Serial.println("Http Event On Connected");
    break;
  case HTTP_EVENT_HEADER_SENT:
    Serial.println("Http Event Header Sent");
    break;
  case HTTP_EVENT_ON_HEADER:
    Serial.print("Http Event On Header, key=");
    Serial.print(event->header_key);
    Serial.print(", value=");
    Serial.println(event->header_value);
            break;
  case HTTP_EVENT_ON_DATA:
    break;
  case HTTP_EVENT_ON_FINISH:
            Serial.println("Http Event On Finish");
            break;
  case HTTP_EVENT_DISCONNECTED:
    Serial.println("Http Event Disconnected");
    break;
  }
}
String listDir(fs::FS &fs, const char * dirname, uint8_t levels){
  String ptr = "<!DOCTYPE html> <html>\n<html>\n<head>\n";
  ptr+="<title>Directory Listing</title>\n";
  ptr+=style_js;
  ptr+="</head>\n<body>\n";

  ptr+="<h1>Listing directory:";
  ptr+=dirname;
  ptr+="</h1>\n";

   File root = fs.open(dirname);
   if(!root){
     //Serial.println("− failed to open directory");
     ptr+="Failed to open directory\n";
     ptr+="</body>\n";
     ptr+="</html>\n";
     return(ptr);
   }
   if(!root.isDirectory()){
     //Serial.println(" − not a directory");
     ptr+="not a directory\n";
     ptr+="</body>\n";
     ptr+="</html>\n";
     return(ptr);
   }

   File file = root.openNextFile();
   while(file){
      if(file.isDirectory()){
	//Serial.print("  DIR : ");
	//Serial.println(file.name());
	ptr+="Dir: ";
	ptr+=file.name();
	ptr+="<br>\n";
         if(levels){
            listDir(fs, file.name(), levels -1);
         }
      } else {
	//Serial.print("  FILE: ");
	//Serial.print(file.name());
	//Serial.print("\tSIZE: ");
	//Serial.println(file.size());
	ptr+="File: ";
	ptr+=file.name();
	ptr+=" ";	
	ptr+=String(file.size());
	ptr+="<br>\n";
      }
      file = root.openNextFile();
   }
   ptr+="</body>\n";
   ptr+="</html>\n";
   return(ptr);
}
String ls_l (fs::FS & fs, const char * dirname, bool htmlHeader, bool htmlFooter, bool htmlTable, bool showSize, bool showDate, uint8_t levels){
  String ptr = "";
  if(htmlHeader) {
    ptr+="<!DOCTYPE html> <html>\n<html>\n<head>\n";
    ptr+="<title>Directory Listing</title>\n";
    ptr+=style_js;
    ptr+="</head>\n<body>\n";
  }
  ptr+="<h1>Listing directory:";
  ptr+=dirname;
  ptr+="</h1>\n";
  File root = fs.open(dirname);
  if(!root){
    ptr+="Failed to open directory";
    if(htmlFooter) {
      ptr+="</body>\n";
      ptr+="</html>\n";
    }
    return(ptr);
  }
  if(!root.isDirectory()){
    ptr+="Not a directory";
    if(htmlFooter) {
      ptr+="</body>\n";
      ptr+="</html>\n";
    }
    return(ptr);
  }
  File file = root.openNextFile();
  while(file){
    if(file.isDirectory()){
      ptr+="Dir: ";
      ptr+=file.name();
      if(showSize) {
	ptr+=" ";
	ptr+=humanReadableSize(file.size());
      }
      if(showDate) {
	time_t t= file.getLastWrite();
	struct tm * tmstruct = localtime(&t);
	ptr+=" ";
	ptr+=asctime(tmstruct);
      }
      if(levels){
	ls_l(fs, file.path(), false, false, htmlTable, showSize, showDate, levels -1);
      }
      ptr+="<br>\n";
    } else {
      ptr+=file.name();
      if(showSize) {
	ptr+=" ";
	ptr+=humanReadableSize(file.size());
      }
      if(showDate) {
	time_t t= file.getLastWrite();
	struct tm * tmstruct = localtime(&t);
	ptr+=" ";
	ptr+=asctime(tmstruct);
      }
      ptr+="<br>\n";
    }
    file = root.openNextFile();
  }
  if(htmlFooter) {
    ptr+="</body>\n";
    ptr+="</html>\n";
  }
  return(ptr);
}

  #ifdef USESECURE
void handle_ls2(httpsserver::HTTPRequest * req, httpsserver::HTTPResponse * res){
  //iotfw_send(200, "text/html",  listDir(LittleFS, "/", 2));
  //LittleFS.end();
  //LittleFS.begin(false);
  res->print(listDir(LittleFS, "/", 2));
}
void handle_ls_l(httpsserver::HTTPRequest * req, httpsserver::HTTPResponse * res){
 params = req->getParams();
  __req=req;
  __res=res;
  std::string value;
  bool showSize = params->getQueryParameter("showSize",value);
  bool showDate = params->getQueryParameter("showDate",value);
  res->print(ls_l(LittleFS, "/", true,true,true,showSize,showDate,2));
}
void handle_ls(httpsserver::HTTPRequest * req, httpsserver::HTTPResponse * res){
  iotfw_send(200, "text/html", lsHTML());
  #else
  //server.send(200, "text/html", listDir(LittleFS, "/", 2));
  server.send(200, "text/html", lsHTML());
  #endif
}
String octet(int a) {
  String s = String(a & 255);
  s += ".";
  s += String((a>>8) & 255);
  s += ".";
  s += String((a>>16) & 255);
  s += ".";
  s += String((a>>24) & 255);
  return(s);
}
#ifdef USESECURE
 void handle_indirectRoot(httpsserver::HTTPRequest * req,  httpsserver::HTTPResponse * res){
     params = req->getParams();
  __req=req;
  __res=res;
  if(DEBUG) {Serial.println("handle indirect root");}
#else
  void handle_indirectRoot() {
#endif
  roothandler();
}
// Make size of files human readable
// source: https://github.com/CelliesProjects/minimalUploadAuthESP32
String humanReadableSize(const size_t bytes) {
  if (bytes < 1024) return String(bytes) + " B";
  else if (bytes < (1024 * 1024)) return String(bytes / 1024.0) + " KB";
  else if (bytes < (1024 * 1024 * 1024)) return String(bytes / 1024.0 / 1024.0) + " MB";
  else return String(bytes / 1024.0 / 1024.0 / 1024.0) + " GB";
}
/* 
Pseudo-variable replacement
supported variables
DISABLE
DNS
DST
EMAIL_PASSWORD
FILELIST
FOOTER_LINKS
FREELittleFS
FROM_EMAIL
GATEWAYIP
HOSTNAME
LOCALIP
NETPICKER
NETSTATE
PSK
SMTP_HOST
SMTP_PORT
SSID
STATUS
STYLE_JS
SUBNETMASK
TIME
TIMESTATUS
TIMEZONE
TOTALLittleFS
TO_EMAIL
TZPICKER
UPDATEKEY
UPDATEURI
USEDLittleFS
USER
USER_DOMAIN
"" %%==%
  user extensions
to be supported:
 */
String varReplace(const String& var) {
  String ret = "__UNSET__";
  if (var == "FILELIST") {
    ret = lsHTML();
  }
  if (var == "FREELittleFS") {
    ret = humanReadableSize((LittleFS.totalBytes() - LittleFS.usedBytes()));
  }
  if (var == "USEDLittleFS") {
    ret = humanReadableSize(LittleFS.usedBytes());
  }
  if (var == "TOTALLittleFS") {
    ret = humanReadableSize(LittleFS.totalBytes());
  }
  if (var == "STYLE_JS") {
    ret = style_js;
  }
  if (var == "FOOTER_LINKS") {
    ret = footer_links;
  }
  if (var == "STATUS") {
    ret = genStatus();
  }
  if (var == "NETPICKER") {
    ret = genNetPicker();
  }
  if (var == "TZPICKER") {
    ret = genTZPicker();
  }
  if (var == "PSK") {
    ret = netTzData.net.psk;
  }
  if (var == "HOSTNAME") {
    ret = netTzData.net.hostname;
  }
  if (var == "UPDATEURI") {
    ret = netTzData.net.updateURI;
  }
  if (var == "UPDATEKEY") {
    ret = netTzData.net.updateKey;
  }
  if (var == "") { // %% returns %
    ret = "%";
  }
  if (var == "NETSTATE") {
    ret = ascNetState();
  }
  if (var == "LOCALIP") {
    ret = octet(WiFi.localIP());
  }
  if (var == "SUBNETMASK") {
    ret = octet(WiFi.subnetMask());
  }
  if (var == "GATEWAYIP") {
    ret = octet(WiFi.gatewayIP());
  }
  if (var == "DNS") {
    ret = octet(WiFi.dnsIP());
  }
  if (var == "SSID") {
    ret = netTzData.net.ssid;
  }
  if (var == "TIME") {
    timeinfo = localtime (&seconds);
    ret=asctime(timeinfo);
  }
  if (var == "TIMESTATUS") {
    ret = ascTimeStatus();
  }
  if (var == "TIMEZONE") {
    ret = netTzData.tz.tz;
  }
  if (var == "DST") {
    if(netTzData.tz.dst){
      ret = "DST";
    }else{
      ret="";
    }
  }
  if (var == "USER") {
    ret = __req->getBasicAuthUser().c_str();
  }
  if (var == "DISABLE") {
    if((__req->getBasicAuthUser().length()>0) && (strcmp("admin",__req->getBasicAuthUser().c_str()) !=0)){
      ret = "disabled";
    }else{
      ret="";
    }
  }
  if (var == "SMTP_HOST") {
    ret = netTzData.net.smtp_host;
  }
  if (var == "SMTP_PORT") {
    ret = String(netTzData.net.smtp_port);
  }
  if (var == "FROM_EMAIL") {
    ret = netTzData.net.from_email;
  }
  if (var == "EMAIL_PASSWORD") {
    ret = netTzData.net.email_password;
  }
  if (var == "USER_DOMAIN") {
    ret = netTzData.net.user_domain;
  }
  if (var == "TO_EMAIL") {
    ret = netTzData.net.to_email;
  }

  /* template
  if (var == "") {
    ret = 
  }
  */
  if((usrTokHandler != NULL) && (ret == "__UNSET__")){
    ret = usrTokHandler(var);
  }
  if(ret == "__UNSET__") {
  // default
  ret="-Err: unknown variable %";
  ret += var;
  ret += "%-";
  } else if (ret=="") {
    ret=" ";
  }
  return ret;
}
String lsHTML() {
  String ptr = "<table><tr><th align='left'>Name</th><th align='left'>Size</th></tr>";
  File root = LittleFS.open("/");
  //if(DEBUG) {Serial.print(__LINE__); Serial.print("root: "); Serial.println(root);}
  //if(DEBUG) {Serial.print(__LINE__); Serial.print("root is directory: "); Serial.println(root.isDirectory());}
  File foundfile = root.openNextFile();
  while (foundfile) {
    ptr += "<tr align='left'><td>" + String(foundfile.name()) + "</td><td>" + humanReadableSize(foundfile.size()) + "</td></tr>";
    foundfile = root.openNextFile();
  }
  ptr += "</table>";
  //if(DEBUG) {Serial.println(__LINE__);}
  return ptr;
}
#ifdef USESECURE
void handleFileUpload(httpsserver::HTTPRequest * req, httpsserver::HTTPResponse * res) {
  params = req->getParams();
  __req=req;
  __res=res;
  httpsserver::HTTPBodyParser * parser;
  parser = new httpsserver::HTTPMultipartBodyParser(req);
  String ptr = "<!DOCTYPE html> <html>\n<html>\n<head>\n";
  ptr+="<title>File Upload</title>\n";
  ptr+=style_js;
  ptr+="</head>\n<body>\n";
  /*
  if (req->getRequestString().c_str() != "/upload") {
    ptr+="Failed</body>\n";
    ptr+="</html>\n";
    iotfw_send(200, "text/html", ptr);
    return;
  }
  */
  size_t fileLength = 0;
  while (parser->nextField()) {
    std::string name = parser->getFieldName();
    std::string filename = parser->getFieldFilename();
    if(filename == "") {ptr += "Skipping null file name"; break;}
    std::string mimeType = parser->getFieldMimeType();
    Serial.print("handleFileUpload: field name='"); Serial.print(name.c_str()); Serial.print("', filename='");
    Serial.print(filename.c_str()); Serial.print("', mimetype='"); Serial.print(mimeType.c_str()); Serial.println("'");  
                 
    // Double check that it is what we expect
    if (name != "data")    {
      Serial.print("Skipping unexpected field");
      Serial.print(name.c_str());
      Serial.println(".");
      break;
    }
    File f;
    String fn = "/" + String(filename.c_str());
    ptr += "Uploading file: ";
    ptr += fn;
    ptr += ", Mime type: ";
    ptr += String(mimeType.c_str());
    ptr += "<br>\n";
    if (LittleFS.exists((char *)fn.c_str())) {
      ptr += "deleting old file<br>\n";
      LittleFS.remove((char *)fn.c_str());
    }
    f = LittleFS.open(fn.c_str(), FILE_WRITE);
    if(DEBUG) {Serial.print("Upload: START, filename: "); Serial.println(fn);}
    ptr += "Opened: " + fn + "\n";
    ptr += String(f);
    ptr += "<br>\n";
    if(!f) {   ptr += "Open Failed!<br>\n";}
    while (!parser->endOfField()) {
      byte buf[512];
      size_t readLength = parser->read(buf, 512);
      f.write(buf, readLength);
      fileLength += readLength;
    }
    f.close();
  }
  res->println("</body></html>");
  delete parser;
  ptr += "Upload Done ";
  ptr += "<br>\n";
  if(DEBUG) {Serial.print("Upload: END, Size: "); Serial.println(fileLength);}
  ptr += " Bytes total: ";
  ptr += humanReadableSize(fileLength);
  ptr += "<br>\n";
  ptr+="</body>\n";
  ptr+="</html>\n";
  iotfw_send(200, "text/html", ptr);
}
 void hash64pw(String user, String pass, unsigned char * encpw){
   String hashin = user + pass;
   byte shaResult[32];
   mbedtls_md_context_t ctx;
   size_t outlen;
   mbedtls_md_type_t md_type = MBEDTLS_MD_SHA256;
   const size_t payloadLength = strlen(hashin.c_str());
   mbedtls_md_init(&ctx);
   mbedtls_md_setup(&ctx, mbedtls_md_info_from_type(md_type), 0);
   mbedtls_md_starts(&ctx);
   mbedtls_md_update(&ctx, (const unsigned char *) hashin.c_str(), payloadLength);
   mbedtls_md_finish(&ctx, shaResult);
   mbedtls_md_free(&ctx);
   mbedtls_base64_encode(encpw, 64, &outlen, shaResult, 32);
   encpw[outlen] =0;
 }
#else
void handleFileUpload() {
  String ptr = "<!DOCTYPE html> <html>\n<html>\n<head>\n";
  ptr+="<title>File Upload</title>\n";
  ptr+=style_js;
  ptr+="</head>\n<body>\n";
  if (server.uri() != "/upload") {
    ptr+="Failed </body>\n";
    ptr+="</html>\n";
    server.send(200, "text/html", ptr);
    return;
  }
  File f;
  HTTPUpload& upload = server.upload();
  String fn = "/" + upload.filename;
  ptr += "Uploading file: ";
  ptr += fn;
  ptr += "<br>\n";
  ptr += "Upload Status ";
  ptr += String(upload.status);
  ptr += "<br>\n";
  if (LittleFS.exists((char *)fn.c_str())) {
    ptr += "deleting old file<br>\n";
    LittleFS.remove((char *)fn.c_str());
  }
  if (upload.status == UPLOAD_FILE_START) {
    ptr += "Start<br>/n";
  }
  //f = LittleFS.open(upload.filename.c_str(), FILE_WRITE);
  f = LittleFS.open(fn.c_str(), FILE_WRITE);
  if(DEBUG) {Serial.print("Upload: START, filename: "); Serial.println(upload.filename);}
  ptr += "Opened: " + fn + "\n";
  ptr += String(f);
  ptr += "<br>\n";
  if(!f) {   ptr += "Open Failed!<br>\n";}
  while((upload.status != UPLOAD_FILE_END) && (upload.status == UPLOAD_FILE_ABORTED) ) {
    ptr += "Upload Status ";
    ptr += String(upload.status);
    ptr += "<br>\n";
    if (upload.status == UPLOAD_FILE_WRITE) {
      if (f) {
	f.write(upload.buf, upload.currentSize);
	ptr += "Write<br>\n";
      }
      if(DEBUG) {Serial.print("Upload: WRITE, Bytes: "); Serial.println(upload.currentSize);}
    }
  }
  if (upload.status == UPLOAD_FILE_END) {
    if (f) {
      ptr += "Final write and Close<br>\n";
      f.write(upload.buf, upload.currentSize);
      f.close();
    }
  }
  ptr += "Upload Status ";
  ptr += String(upload.status);
  ptr += "<br>\n";
  if(DEBUG) {Serial.print("Upload: END, Size: "); Serial.println(upload.totalSize);}
  ptr += " Bytes total: ";
  ptr += humanReadableSize(upload.totalSize);
  ptr += "<br>\n";
  ptr+="</body>\n";
  ptr+="</html>\n";
  server.send(200, "text/html", ptr);
}
#endif
 CronID_t iotfw::runIn(int val, int units, OnTick_t handler) {
   time_t seconds;
   time_t newSeconds;
   struct tm * newTimeinfo;
   char newCronTime[18];
   time_t offset;
   if(units==iotfw::month) {
     offset=0;
   } else {
     offset = val*units;
   }
   //if(DEBUG) {Serial.println(__LINE__);}
   seconds = now();
   newSeconds = seconds + offset;
   newTimeinfo = localtime (&newSeconds);
   //if(DEBUG) {Serial.print(__LINE__);Serial.printf(" seconds %d, newSeconds %d, offset %d\n",seconds,newSeconds,offset);}

   if(units==iotfw::month) {
     newTimeinfo->tm_mon = (newTimeinfo->tm_mon + val) % 12;
   }
   //if(DEBUG) {Serial.print(__LINE__);Serial.println(asctime(newTimeinfo));}
   sprintf(newCronTime,"%d %d %d %d %d *\0",
	 newTimeinfo->tm_sec,
	 newTimeinfo->tm_min,
	 newTimeinfo->tm_hour,
	 newTimeinfo->tm_mday,
	   newTimeinfo->tm_mon + 1);
   //if(DEBUG) {Serial.print(__LINE__); Serial.println(newCronTime);}
   return(Cron.create(newCronTime, handler,true));
 }
