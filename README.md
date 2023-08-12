#IOTFW
##A framework for the internet of things on ESP32.
##2023 jmlzone
Having completed a few projects on ESP32, I wanted to have a b ase library that took care of the 'required basics'
this framework does what I like as a foundation of an ESP32 IOT project.

- Provide basic AP accesspoint and captive server functionality.
- Provide easy connection to your network with fallback to access point.
- Provide easy OTA Over the air update capability.
- Provide a way to send SMS/Email notifactions.
- Provide time via NNTP.
- Provide a way to run things on timers and calendars.
- Provide HTTP and HTTPS servers.
- Provide basic upload and download capability.
- Provide an encrypted password Mechansim for https access.
- Does not require coding SSID and password in Arduino code.
- provide a basic and extensible method for variable substitution in HTML/String(s)

## Examples
- iotSwitchDemo.
A simple example of toggling a pin/led using this framework.

- textTest
This was a simple example I did to debug how to use gmail.  (does not use the framework)

-GenSmart
This is the firmware for the smart controllerboard I added to my generator.

## Library dependencies
The framworks uses:
- WiFi
- WiFiClientSecure
- HTTPS_Server_Generic
- DNS
- SPI
- LittleFS
- WiFiUdp
- TimeLib
- HttpsOTAUpdate
- CronAlarms
- unistd
- string
- mbedtsl/md
- mbedtls/base64

## As of this version, it is working.
There are still improvments I want to add.
No warantee is expressed or implied.

#Basic Use
If you have all the dependant libraries on your system, Download this and the iotSwitchDemo.
build and flash onto your ESP32.

Upon Boot you will see a new network iotfw_****
Connect to the network and bring up the root page.  Any access should bring you to the root page.
The Configure Network and time zone page is the main control point for the framework.
The configuration page is broken into serveral section.
1 Status.  The status area gives basic information about the configuration.
2 Network settings.
--It should populate with the list of networks it scanned.  You can select your network and add your network key/password.
--You can also set the hostname here.
--If you have a webserver where you will have your binarys for OTA update you can specify that with the Update URI.
--The email area is the configuration for notifications in your code.
--The Cancel - Save - Reboot buttons do what they say.  Save after you have configured your network and it will reoot.  If it can not connect to the network you save, it will fall back to access point mode.
3 File upload area.  This area alows you to upload files.  I would suggest you upload a favicon.ico file and a gmailCert.txt file if you are using the email/sms functions.
4 Change or add password area.
With no passwords set, the fall back is user 'admin' with password 'secret'. I suggest you change the admin password first.  When logged in as admin you can add new users by typing a new user name and password.  Non admin users can only change their password.
5 The Email/SMS testing area will try to send an email/SMS based on the confiruration above.

After the reboot and connection to your network this page will only be accessable though HTTPS using the username(s) and password(s) you have specified.  If it can not connect to your network, it will fall back to access point mode and this page is openly accessable through HTTP.
Clicking the login button will transfer to the HTTPS server when on your network.  For devices using the framework its up to the designer to select what is accessable thbrough HTTP vs HTTPS.

More documentation later.

#Variables handled by the basic framwork for String or HTML substitution  wen used with the 'Tok' methods.
-DISABLE
-DNS
-DST
-EMAIL_PASSWORD
-FILELIST
-FOOTER_LINKS
-FREELittleFS
-FROM_EMAIL
-GATEWAYIP
-HOSTNAME
-LOCALIP
-NETPICKER
-NETSTATE
-PSK
-SMTP_HOST
-SMTP_PORT
-SSID
-STATUS
-STYLE_JS
-SUBNETMASK
-TIME
-TIMESTATUS
-TIMEZONE
-TOTALLittleFS
-TO_EMAIL
-TZPICKER
-UPDATEKEY
-UPDATEURI
-USEDLittleFS
-USER
-USER_DOMAIN
