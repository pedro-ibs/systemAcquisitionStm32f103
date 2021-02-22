# system Acquisition Stm32f103

## Resumo
Esse é um projeto com o objetivo de desenvolver um sistema de aquisição de dados utilizando o microcontrolador stm32f10, focando apenas no código embarcado no micro.


## **Perparando o ambiente, Configurações visuais e extensões*

pacotes para serem instalados:

```
sudo apt update -y
sudo apt upgrade -y
sudo dpkg --add-architecture i386
sudo apt install gcc make cmake dkms build-essential libusb-1.0.0-dev -y
sudo apt install libusb libusb-dev  libusb-1.0-0 libusb-1.0-0-dev -y
sudo apt install gcc-arm-none-eabi stm32flash -y
sudo apt install python3 -y
sudo apt install python3-pip -y
sudo apt install flatpak -y

flatpak install --from https://dl.flathub.org/repo/appstream/com.visualstudio.code.flatpakref

```
No VScode instale algumas extensões, principalmente a PlatformIO. use o arquivo [settings.json](./config/settings.json) para configurar o ambiente, é preciso mover este arquivo apos instalar todas as extensões.



## Extensões visuais

* [Roseate](https://marketplace.visualstudio.com/items?itemName=endorfina.roseate), no [settings.json](./settings.json) há configurações para coloração do código **caso não use esse tema talvez seja preciso alterar ou comentar essas configurações**  
```
/*  configurações de cores do código  */
"editor.semanticTokenColorCustomizations":{
	[ ... ]
}
```

## Extensões auxilares
* [Template Generator](https://marketplace.visualstudio.com/items?itemName=prui.template-generator-vscode), cria templates de arquivos fontes .h .c .cpp. isso deve ser usado pada manter a padronização do código. Use  *ctrl+p*, e de o comando *> Open Templates Folder*, para abri a pasta ondes os arquivos template vão ser criados copie [esses](./config/templates/) arquivos para lá.

* [TODO Highlight](https://marketplace.visualstudio.com/items?itemName=wayou.vscode-todo-highlight), destaca o termo **TODO:**
server para comentar tarefas que precisam ser feitas no código.

* [Code Spell Checker](https://marketplace.visualstudio.com/items?itemName=streetsidesoftware.code-spell-checker), server para corrigir textos.
