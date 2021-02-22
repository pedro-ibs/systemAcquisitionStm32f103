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
Crie uma pasta em /opt/eclipse e transfira ela para o usuário comum, em seguida  baixe todos o  [Eclipse IDE cdt](https://www.eclipse.org/cdt/): `Eclipse IDE for C/C++ Developers (includes Incubating components) ` mova todos os arquivos para /opt/eclipse, após mover os arquivos de permissão de execução do eclipse, aconselho utilizar alguma ferramenta do sistema para criar um lançador na interface gráfica, como um criador/editor de menu.


```
sudo mkdir eclipse
sudo chown nome_usuario eclipse
cd /opt/eclipse/
sudo chmod +x eclipse
``` 

Em no menu `Help/Install new software` cliqui em add instale o  `http://download.eclipse.org/tools/cdt/releases/VERSÃO_DO_ECLIPSE_QUE_BAIXO`, irá encontrar o link certo [QUI!!](https://www.eclipse.org/cdt/downloads.php), aconselho a marcar todos os pacotes para instalação. Após a instalação concluída e o eclipce reiniciar repita este procedimento para: `http://gnuarmeclipse.sourceforge.net/updates`, e também aconselho a marcar todos os pacotes para instalação Aponte a pasta eclipse deste projeto como workspace.

## **Gravador**

Veja a pasta [stlink](./arquivos/stlink/) em arquivos!



### **Protocolo** 
Cada ponto tem 12 Bits das medidas portanto é preciso utilizar 2 bytes (0xFF) totalizando 16 bits **0bddddDDDD 0bDDDDDDDD** onde **dddd** é o endereço do ADC, e **DDDD DDDDDDDD** é o valor da amostra por exemplo *00030000 11111111* é a leitura de 255 do adc 3
