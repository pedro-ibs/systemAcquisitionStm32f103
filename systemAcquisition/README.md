# M41 REALISE

## **FIRMWARE REALISE**
 * MA_1.09
    * Lançamento para testes.

 </br>

 * MA_1.10
    * Correção do comado do comando **QCFG="nwscanmode",3,1** para  **QCFG="nwscanmode",0,1**.
 
 </br>

 * MA_1.11
    * Pré boot alterado.
      * Sequânica de escaneamento de rede modificado para as tecnologias **LetCat M1** e **GSM**.
    * Correção da leitura do comando link, Corrigindo o teste do link entre o GPRS e servidor.
    * O **Evento 0C** de mudança de ip agora é **atuado** para a **mudança de IP** e não mais para LAC e CELLID.
    * Os pacotes comuns levam **CELLID** e **LAC**


### Pré boot para versão MA_1.10 
```
[15:02:10:278] AT+CFUN=0␍␊
[15:02:10:278] AT+CFUN=0␍␍␊
[15:02:10:442] OK␍␊
[15:02:10:579] ATI␍␊
[15:02:10:579] ATI␍␍␊
[15:02:10:579] Quectel␍␊
[15:02:10:579] BG95-M3␍␊
[15:02:10:579] Revision: BG95M3LAR02A03␍␊
[15:02:10:604] ␍␊
[15:02:10:604] OK␍␊
[15:02:10:976] AT+QCFG="nwscanmode",0␍␊
[15:02:10:976] AT+QCFG="nwscanmode",0␍␍␊
[15:02:10:992] OK␍␊
[15:02:11:280] AT+QCFG="nwscanseq",0201␍␊
[15:02:11:280] AT+QCFG="nwscanseq",0201␍␍␊
[15:02:11:295] OK␍␊
[15:02:11:634] AT+QCFG="iotopmode",0␍␊
[15:02:11:634] AT+QCFG="iotopmode",0␍␍␊
[15:02:11:663] OK␍␊
[15:02:11:940] AT+CFUN=1,1␍␊
[15:02:11:940] AT+CFUN=1,1␍␍␊
[15:02:11:940] OK␍␊

```

 </br>

 * MA_1.12
    * **Correção** do **pre boot**  para os módulos **M95** e **UG96**.

 </br>

 * MA_1.13
   * Comando AT+QCFG="band",F,8000004,8000004. alterado para Comando AT+QCFG="band",F,8000097,8000097
   * Correção do evento C7 troca de IP
   * Adicionado evento **RB**, relatório periódico de LAC e CELLID 

 </br>

 * MA_1.14
    * Primeiro evento **RB** corrigido. 
    * Controle da saida corrigido.
    * adicionado novos campos no pacote de configuração:
      * TEC: tecnologia que o moduló está conectado
      * BAND: Banda que o moduló está usando
      * CH: Canal que o moduló está usando
    * O evento RB é gerado 3 vezes a cada 6H ele levará informações como:
      * LAC e CELLID
      * CANAL e BANDA
      * Tecnologia 

 </br>

 * MA_1.15
    * Padrão do evento **RB** Alterado, apos a informação 10, 20, 30, 40, (LAN, 2G, 3G, 4G) terá os  seguintes dados: 
      * LAC, CELL ID, ACT, OPER  e BAND
    * O pacote de configurações agora leva os seguintes  grupos de informações.
      * Cabeçalho, de a cordo com o servidor destino
      * Configurações e dados comuns aos servidores
      * Configuração respectiva so servidor destino
    * Remoção dos campos QSPN. e CH
    * Pacote corrigido
    * Troca na regra de contagem de falha para um servidor. Agora a contagem é feita a cada tentativa do mesmo ciclo de envio, caso a contagem fosse por falha de um ciclo ele iria levar um tempo muito grande para chegar em 50.
    * Quando o servidor 1 (de prioridade) estiver em falha, ou seja, quando a prioridade estiver trocada para o servidor 2: o servidor volta apos um evento de treg.

 </br>

 * MA_1.16
    * Comando NETINFO, dispara o evento **RB**
    * Evento RB corrigido para o modulo M95, alteração:
      * LAC, CELL ID, ATI, OPER  e BAND
    * Ordem das informações corrigido do evento RB

 </br>

 * MA_1.17
    * Correção do evento RB **RB** para os módulos, populado com as informações certas 
      * M95
      * UG96
    * Boot corrigido para módulo UG96
    * 20 segundos após um comando de configuração o dispositivo irá reiniciar
    * IHM ativada
      * **NOTA 1** Para o led DATA que indica o envio e o recebimento de informações pode haver um assincronismos ou perda de informação visual em detrimento da velocidade de envio dos módulos. Por exemplo a A LAN é geralmente tão rápida em enviar e receber pacotes que a IHM não consegue acompanhas, neste caso o lede apenas pisca quando recebe o ACK.
      * **NOTA 2** A qualidade de sinal é mostrada apenas quando o GPRS é online, por que o modulo lan não tem este atributo.

 </br>

 * MA_1.18
    * Agora a placa reinicia 20 segundos apos uma configuração
    * Agora a placa reinicia imediatamente após o comando RSTPROG
    * O segundo servidor agora só funciona com SM=2
    * Comando **RSTPROG** por sms bloqueado

 </br>

 * MA_1.19
    * Os códigos a seguir passaram por refatoração, para aliviar o uso de memoria, permitindo o uso de um novo método de aquisição do contactId.
      * Modulo de controle de eventos
      * Modulo de controle de Entrada e saída 
      * Modulo de controle de configuração do dispositivo
      * Modulo de controle de escalonamento de eventos 
      * *Modulo de controle de aquisição do contactId*
      * *Modulo de controle de aquisição de tons dtmf*
      * Modulo de controle da comunicação
      * Gerador de pacotes
      * Funções do modulo lan
      * Funções do modulo GPRS
    * O Driver do FreeRTOS foi atualizado, essa versão permite ou não o uso do DMA para acessar o ADC
    * *Falta implementar e ou testar*
      * Eventos de treg 
      * Teste de troca de prioridade
      * Teste de ciclos de eventos do GPRS
      * Trava da LAN em caso de perda de internet
      * Corrigir leds
      * Observar problema de trava

</br>

* MA_1.20
  * Correção de repetição de pacotes (quando seguido de um contactId)
  * Correção da trava do dispositivo
  * Implementando evento TREG local
  * Logica de reinicio do GPRS corrigido
  * Logica te trava da lan implementada
  * Troca de prioridade corrigida

</br>

* MA_1.21
  * Correção da visualização do status da placa via IHM
  * Correção do modulo signal
  * Tentativa de correção do UID que gera o código IMEI
  * Pacote de programação leva todos os dados mesmo os vazios


Formato do código serial 
```
A 37 95 38 34 39 37 31 04 47 31 39 30   [ 1.21  ] apenas para testes
A                37 31 04 47 31 39 30   [ -1.20 ]

```

</br>

* MA_1.22
  * Removido o comando de configuração de porta da LAN apenas para testes do problema de conexão DHCP

</br>

* MA_1.23
  * alteração da *MA_1.22* revertida
  * Utilizar a resposta do comando *AT+CIMI* para apn automática
  * código serial agora tem novo formato:


Server  | ID                | UID         | Código Serial
:------ | :---------------- |:----------- | :---------------------
A       | 0111              | 3795383439  | A01113795383439
B       | 0112              | 3795383439  | A01123795383439


</br>

* MA_1.24
  * Porta LPT por padrão virá ente 7000 a 7512
  * Endereço OUI do endereço mac pro padrão começará com 4A.19.1B.XX.XX.XX últimos dígitos são carregados de acordo com o UID do micro  

</br>

* MA_1.25
  * mudança no padrão do timestamp para DD/MM/AAAA-HH:mm ( apenas no pacote )
  * Correção na atualização do treg.
  * Correção na captura do contactId
    * Foi observado que para o painel CADEX é interessante utilizar MAG=2 ou maior, com MAG=1 há falha na leitura do contact ID.
  * Adicionado um campo TOUT (timeout), que determina a fluxo e a janela de tempo para o envio do kissoff após receber um ACK:

<br>

**Funcionamento do timeout TOUT=0**<br>
Ao receber um AKC de o dispositivo precisa detectar tons dtmf e após 200ms a 500ms (**t0**) dá ultima detecção o KISSOFF é enviado para o painel

```
>----|ACK|----|DTMF|-|t0|-|KISSOFF|---->
```

**Funcionamento do timeout TOUT>0**<br>
Nesse caso o TOUT determina a janela "possível" de envio do KISSOFF, ou seja, caso o AKC chegue antes do estouro de TOUT o KISSOFF é enviado. O contador para o TOUT é iniciado após  200ms a 500ms (**t0**) dá ultima detecção de um DTMF.  
```
>----|DTMF|-(t0)-|TOUT|-|KISSOFF|---->
>--------------|-AKC--|-------------->
```

</br>

* MA_1.26
  * correção na geração de eventos quando é enviado comandos por sms
  * Trava do RSTPROG corrigida

</br>

* MA_1.27
  * Correção na detecção de comandos via udp
  * Correção nos comando do comando armado / desarmado 
  * Correção doo estado do painel: leitura de armado / desarmado
  * correção da captura do contactId ( uma tentava ainda não consigo fazer testes massivos  **coloquei o mag do CADEX como 15** )

</br>

* MA_1.28
  * Melhoria da aquisição do DTMF
  * O Painel só entra em processo de discagem quando um dos módulos estiverem livres (online, plugados e não trabalhando)

</br>

* MA_1.29
  * RSTPROG por serial corrigido
  * RSTPROG habilitado por sms e serial (UDP não)

</br>

* MA_130
  * Tentativa de corrigir detecção de contactId

</br>

* MA_131
  * Tentativa de corrigir detecção de contactId ( teste massivo por um longo período )
  * Reinicio automático  a cada 24h, a dispositivo só reinicia caso não haja eventos a serem enviados

</br>

* MA_132
  * Sistema reinicio do tarefa de aquisição se DTMF adicionada, no Inicio do processo que aquisição do contactId e no Handshake. 

</br>

* MA_133
  * Modulo M95 agora aceita domínio ou ip no campo IP e IP2
  * Campos de IP aumentados para 40 bytes
  * Byte de sinalização da entrada / painel, foram invertidas
  * Adicionado números temporários para testes 

</br>

* MA_134
  * Correção do RSTPROG por SMS

</br>

* MA_135
  * FW versão realise



</br><hr><br>
