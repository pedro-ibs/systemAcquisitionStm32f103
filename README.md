# system Acquisition Stm32f103

## Resumo
Esse é um projeto com o objetivo de desenvolver um sistema de aquisição de dados utilizando o microcontrolador stm32f10, focando apenas no código embarcado no micro.


## **Perparando o ambiente**

pacotes para serem instalados:

```
sudo apt update -y
sudo apt upgrade -y
sudo dpkg --add-architecture i386
sudo apt install default-jre default-jdk -y
sudo apt install gcc make cmake dkms build-essential libusb-1.0.0-dev -y
sudo apt install libusb libusb-dev  libusb-1.0-0 libusb-1.0-0-dev -y
sudo apt install gcc-arm-none-eabi stm32flash -y
sudo apt install avrdude binutils-avr gcc-avr avr-libc gdb-avr -y

```

### **Protocolo** 
Cada ponto tem 12 Bits das medidas portanto é preciso utilizar 2 bytes (0xFF) totalizando 16 bits **0bddddDDDD 0bDDDDDDDD** onde **dddd** é o endereço do ADC, e **DDDD DDDDDDDD** é o valor da amostra por exemplo *00030000 11111111* é a leitura de 255 do adc 3
