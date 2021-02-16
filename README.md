# system Acquisition Stm32f103

## Resumo
Esse é um projeto com o objetivo de desenvolver um sistema de aquisição de dados utilizando o microcontrolador stm32f10, focando apenas no código embarcado no micro.

#### **característica**
  * Frequência esperada de 200khz
  * Sistema base FreeRTOS


### **Protocolo** 
Cada ponto tem 12 Bits das medidas portanto é preciso utilizar 2 bytes (0xFF) totalizando 16 bits **0bddddDDDD 0bDDDDDDDD** onde **dddd** é o endereço do ADC, e **DDDD DDDDDDDD** é o valor da amostra por exemplo *00030000 11111111* é a leitura de 255 do adc 3
