## Iniciar Cliente vía Netcat
```nc localhost <port>```
```pass <password>```
```nick <nickname>```
```user <name> 0 * <name>```
ejemplo
> nc irc.irc-hispano.org 6667
> NICK testnicK
> USER testuser 0 * :DavidTest
< PING : TE ENVIA UN CÓDIGO
<:ullapool.chathispano.com 396 testnicK njC.j18.j49ElA.virtual :is now your displayed host
<:ullapool.chathispano.com MODE testnicK :+xwT
> PONG : REPONDES CON EL MISMO CÓDIGO

## Documentación del protocolo
https://modern.ircdocs.horse/

## Iniciar Cliente vía Software de terceros
https://hexchat.github.io/ <- Yo instalé este
https://irssi.org/