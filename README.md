# SymmetricKeyManagement
# First Step  integrates wolfssl and the project to accomplish crypto operation
In this prohject we have used wolfssl for crypto operations such as encryption and key generation. 
To integrate wolfssl to the project, it is necessary to compile and integrate it properly.
Following are the steps for integration.

1) Download wolfssl-4.0.0.zip from the downloads page: https://www.wolfssl.com/download/
2) Unzip and cd into wolfssl-4.0.0/ run configure and make using gcc -fPIC
3) install the library
4) compile gcc client.c -o client -fpie -rdynamic -I${WOLF_INSTALL_DIR}/include -L${WOLF_INSTALL_DIR}/lib -lwolfssl
5) compile gcc server.c -o server -fpie -rdynamic -I${WOLF_INSTALL_DIR}/include -L${WOLF_INSTALL_DIR}/lib -lwolfssl
5) Export the library path with command export LD_LIBRARY_PATH="${LD_LIBRARY_PATH}:${WOLF_INSTALL_DIR}/lib" so the app can find the library at run time
6) run the server.c and connect to it with the client.c which are compiled with -fpie -rdynamic

# Second Step is to create a virtual network on ns-3 which represents LoraWan characterstics:
The scripts for creating the following ns-3 network located in LoraWanNs3 folder
![loraWan Virtual Network](https://github.com/fiu-awl/SymmetricKeyManagement/blob/master/loraNetwork.png)

# To reflect Lora charectestics in terms of latency and bandwith as parameters. Please refer the following work,
We have used spreading factor 7 for urban and spreading factor 9 for rural settings.

Adelantado, Ferran, et al. "Understanding the limits of LoRaWAN." IEEE Communications magazine 55.9 (2017): 34-40.
