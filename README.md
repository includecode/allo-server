# Snowpack
This program is a simulation on connection between users throught a proxy server.

How to use this program

1. Unzip file

2. Go to directory "Allo-server" where "src" and "inc" are located

 
**********
 
 Usage:
    ./app   <mode>   [secret]
    mode          : Proxy / User
    secret        : Optional secret for this (User) device
    Example       : ./app   User  Sn0wp@ck
    This example will run as User device with its secret value Sn0wp@ck
    Secret supplied for Proxy devices are ignored
**********

When a connection is successful with another client, ,they can exchane message.
Tip: use ==> ECHOREPLY msg <== for the receiver to send you back the message 'msg'
