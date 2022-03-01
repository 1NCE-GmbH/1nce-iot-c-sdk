# 1NCE IoT C SDK

## Overview
**1NCE IoT C SDK** for Embedded C is a collection of C source files under the MIT Open source license that can be used in embedded applications to connect to benefit from diffrent services in 1NCE OS. it contains MQTT Onboarding, CoAP Onboarding and translation service. This SDK is independet of platform, can customer used in any device work with standard C. 

## License
The 1NCE IoT C-SDK are licensed under the MIT open source license.

## Features
1NCE IoT C SDK allow customers a seamless setup and use of all features as part of 1NCE OS. 
1NCE IoT C SDK contains the following services: 
![Alt text](./docs/doxygen/images/overview.png?raw=true "overview")
### MQTT Onboarding
The connection to AWS IoT core is mainly done through MQTT, and Devices are created as "Things" that can have various attributes to define their functionality. After creating a representation of the “thing” in AWS IoT Core, a certificate, private key, and public key are generated for the device to enable secure communication to the cloud. In addition, policy configuration is needed to determine what are the permissions granted for the device. In MQTT context, this can for example determine the topics that the device can Publish and/or Subscribe to.
![Alt text](./docs/doxygen/images/mqtt_onboarding.png?raw=true "MQTT Onboarding")
### CoaP Onboarding
The 1NCE IoT c SDK also provides onboarding credintials for IoT devices communicating through DTLS. In this case the device recieves a DTLS Identity and a Pre-Shared Key (PSK) that can be used to establish secure connection to the CoAP endpoint of 1NCE Data Broker.
![Alt text](./docs/doxygen/images/coap_onboarding.png?raw=true "CoAP Onboarding")
### Translation Service (Binary conversion language)
The translation service aims to minimize the payload size sent from the device to a simple byte array that can be converted to JSON Format. The resulting message is then sent using MQTT via the Data broker. Translating the byte array is done using Binary Conversion language which splits the array into a sequence of values defined in a translation template.
![Alt text](./docs/doxygen/images/translation_service.png?raw=true "translation service")
## Versioning
1NCE IoT C SDK releases will follow a [Semantic versioning](https://en.wikipedia.org/wiki/Software_versioning#Semantic_versioning)
Given a version number MAJOR.MINOR.PATCH, increment the:
* MAJOR version when you make incompatible SDK changes,
* MINOR version when you add functionality in a backwards compatible manner,
* PATCH version when you make backwards compatible bug fixes.

## Getting start 

**This section shows you:**

* How to get certificate using 1nce onboarding end point.
* How to process the certificate to make it usable for AWS services.
* How to get the identity and pre-shared key using 1nce onboarding end point. (for CoAP application).
* How to use 1NCE Translation Service to reduce the data (and/ or Energy) consumption. 

### The Scenario

The Example shows how to set up and integrate 1nce SDK in Embedded Application written in C.

This Example includes two cases: 
* Offload certificates.
* Unload certificates.

The Examples used FreeRTOS as First Version.


### Prerequisite Tasks

To set up and run the SDK you must first complete these tasks:

- Install gcc to compile the project. For more information, see the https://gcc.gnu.org/install/index.html. you can find the downloads of compiler.

#### Contents
* [Step1: Clone Repository](#Step1_Clone_Repository_38)
* [Step2: Implement abstract functions](#Step2_Implement_abstract_functions_52)
* [Step3: Integrate SDK in your Application](#Step3_Integrate_SDK_in_your_Application_93)
* [Step4: Run your Application](#Step4_Run_your_Application_122)
## Step1: Clone Repository 
you clone the SDK for Embedded C in your project using git clone
To clone using HTTPS
```
git clone https://github.com/1NCE-GmbH/1nce-sdk.git --recurse-submodules
```
Using SSH
```
git clone git@github.com:1NCE-GmbH/1nce-sdk.git --recurse-submodules
```
If you have downloaded the repo without using the ```--recurse-submodules``` argument, you need to run:
```
git submodule update --init --recursive
```
### Step2: Implement abstract functions
You need to implement your four tls functions how you want to access to our end point onload or offload and also depending on your modem. 

* nce_TLS_Connect
* nce_TLS_send
* nce_TLS_recv
* nce_TLS_Disconnect

To implement your functions, we recommand to follow the examples.
#### Examples Quectel BG96
###### 1. Offload Certificate: 
for the Implementation we use the psuedo code: 
```
TlsTransportStatus_t TLS_Connect_impl(NetworkContext_t *pNetworkContext,
			                          const char *pHostName, uint16_t port,
			                          const NetworkCredentials_t *pNetworkCredentials,
			                          uint32_t receiveTimeoutMs, 
			                          uint32_t sendTimeoutMs)
```
1- [configure the onboarding socket](https://github.com/1NCE-GmbH/blueprint-freertos/blob/94b2f0a364c958df57fe75e969aeef674fece6ba/libraries/3rdparty/NCE_SDK/nce_bg96_configuration.c#L41)
2- Create TCP Socket.
3- Connect the socket with the endpoint ```ONBOARDING_ENDPOINT``` in port ```ONBOARDING_PORT```
```
int32_t TLS_send_impl(NetworkContext_t *pNetworkContext,
			          const void *pBuffer,
			          size_t bytesToSend)
```
Create send via TLS for Quectel BG96 we can call At command ```AT+QSSLSEND``` 
```
int32_t TLS_recv_impl(NetworkContext_t *pNetworkContext,
                      void *pBuffer,
			          size_t bytesToRecv)
```
Create Recv function via TLS for Quectel BG96 we can call At command ```AT+QSSLRECV``` 
```
void TLS_Disconnect_impl(NetworkContext_t *pNetworkContext)
```
1- Call socket shutdown function to close the connection
2- Free mbedTLS contexts.
###### 2. Onload Certificate:
For the Unload Certificate we can reffer to [using_mbedtls in FreeRTOS Demo](https://github.com/FreeRTOS/Lab-Project-FreeRTOS-Cellular-Demo/blob/main/source/coreMQTT/using_mbedtls.c)
### Step3: Integrate SDK in your Application
1NCE SDK is simple to integrate in every Embedded App written with C, To begin, you neet to define an object type tls_api and the affect to thier variable the networkContext and implimented functions as shown in the example bellow. 
```
    tls_api xtls_api;
    xtls_api.pNetworkContext= &xNetworkContext;
    xtls_api.conn=TLS_Connect_impl;
    xtls_api.disconn=TLS_Disconnect_impl;
    xtls_api.tlssend=TLS_send_impl;
    xtls_api.tlsrecv=TLS_recv_impl;
    nce_sdk(&xtls_api,1);
```
then you have the Root certificate store in ```rootCA``` , Client certificate in ```clientCert``` , device private Key in ```prvKey```
Thingname, endpoint and topic name also stored in ```nceThingName ```,```nceEndpoint```,```nceExampleTopic``` respectively.
nce_sdk function had two functionality the first one is to plug and play with MQTT and also with CoAP protocol to use DTLS you need just to change the 1 to 0 
```
    nce_sdk(&xtls_api,0);
```
then we can have the pre-shared key and identity stored in ```psk``` and ```pskIdentity```

### Step4: Run your Application
Run your code in ISO C99 

## Generating documentation
The Doxygen references were created using Doxygen version 1.9.2. To generate the Doxygen pages, please run the following command from the root of this repository:
```
doxygen docs/doxygen/Doxyfile
```

## Contributing
See [CONTRIBUTING.md](https://gitlab.com/1nce-tech/cons/embedded/1nce_sdk/-/blob/main/CONTRIBUTING.md) for information on contributing