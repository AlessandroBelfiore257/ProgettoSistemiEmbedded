# Smart Bridge

Autore: Alessandro Belfiore

## Descrizione
La proposta di progetto si focalizza sull’ideazione e la conseguente costruzione di un ponte intelligente situato in una località marittima, la cui funzione primaria è la gestione efficiente del traffico marittimo e stradale nella suddetta area, con l’obiettivo di rendere più 
scorrevole la circolazione. 
Il ponte possiede la funzionalità di agire in autonomia sui propri processi di apertura e chiusura, sulla base dell’eventuale presenza di imbarcazioni in attesa di attraversarlo e la rispettiva quantità, con lo scopo di adattarsi alle caratteristiche del traffico in tempo reale. 
Ad esempio, nel caso in cui  non ci fossero imbarcazioni in attesa, il ponte rimarrebbe chiuso, consentendo così il passaggio su strada ininterrottamente. 
D’altra parte, se il numero di imbarcazioni in attesa superasse una soglia precedentemente stabilita, il ponte si aprirebbe per far fluire il traffico marittimo.
Inoltre, il tempo in cui il ponte rimane aperto è determinato dal numero di imbarcazioni che attendono di attraversalo: ad un’unica barca in attesa corrisponderà una durata di apertura stabilita in precedenza, mentre se la quantità di barche aumenta, la suddetta durata aumenterà rispettivamente. 
In aggiunta, per agevolare ulteriormente lo scorrimento del traffico, il ponte considera anche le condizioni delle maree in tempo reale. Quest’ultimo, infatti, è in grado di memorizzare l’altezza dell’attuale livello dell'acqua per valutare lo spazio che intercorre fra quest’ultima e il ponte chiuso, con l’obiettivo di permettere alle imbarcazioni di poterlo oltrepassare in base alle proprie dimensioni, senza che questo si apra e quindi senza far attendere inutilmente i veicoli soprastanti. 
A tale scopo, per comunicare a coloro che attraversano il ponte via mare le raccomandazioni relative all’altezza accettabile per permettere l’attraversamento in sicurezza, viene utilizzato un display posto sul ponte. 
Osserviamo che durante le basse maree il passaggio è consentito a tutte le imbarcazioni, nel rispetto delle condizioni di altezza relative all’attraversamento precedentemente descritte. 
All’aumentare del livello dell'acqua, gradualmente,  tramite il display viene sconsigliato il passaggio alle imbarcazioni più alte, fino al raggiungimento della condizione di alta marea che vieta il passaggio a qualunque tipo di imbarcazione.

## Lista materiali
- 1 Arduino UNO
- 2 LED, uno verde ed uno rosso per il semaforo veicoli
- 2 resistori da 220ohm
- 3 micro servomotori, uno per la sbarra veicoli e 2 per i bracci del ponte
- 1 buzzer
- 1 sensore di livello dell'acqua per le maree
- 2 sensori ad ultrasuoni HC-SR04 per il conteggio delle barche
- 1 display LCD
- diversi jumper di lunghezza variabile
- 2 breadboard da 830 pin
- 1 mini breadboard da 170 pin
- polistirolo per i bracci del ponte
- legno
- colla
- scotch nero
- bicchierino da caffè
- cucchiaino da caffè (in plastica)

## Librerie aggiuntive
- Ultrasonic
- Servo
- TaskScheduler
- LiquidCrystal_I2C

## Circuito
  ![circuito](https://github.com/AlessandroBelfiore257/ProgettoSistemiEmbedded/assets/73615267/feace116-4799-4990-80a4-540db4f57e7d)

## Fotografie

- Apertura ponte
  ![ponteAperto](https://github.com/AlessandroBelfiore257/ProgettoSistemiEmbedded/assets/73615267/719b2548-2dec-4b61-86b8-56ad704bd812)
  ![ponteAperto2](https://github.com/AlessandroBelfiore257/ProgettoSistemiEmbedded/assets/73615267/5b21038c-1641-415e-b9cd-28358c6ddb3a)
 
- Alto
  ![alto](https://github.com/AlessandroBelfiore257/ProgettoSistemiEmbedded/assets/73615267/e69f1c84-1e3c-4f40-adb9-e1db1b0049b0)
  
- Fronte
  ![fronte](https://github.com/AlessandroBelfiore257/ProgettoSistemiEmbedded/assets/73615267/7dc08228-8445-40d0-bd4e-788bfe66b6c0)
  
- Laterale
  ![laterale](https://github.com/AlessandroBelfiore257/ProgettoSistemiEmbedded/assets/73615267/26987972-5fed-4ece-bf9e-2bd9ae42a6d4)
 
- Sotto
  ![sotto](https://github.com/AlessandroBelfiore257/ProgettoSistemiEmbedded/assets/73615267/19891c3f-9f3e-4016-9794-bbaf3cc801e6)

- Idea progettuale su carta
  ![IdeaProgettuale](https://github.com/AlessandroBelfiore257/ProgettoSistemiEmbedded/assets/73615267/ece47596-ab2a-483f-9525-df2faefd5e04)

## Riflessioni fatte in fase di progettazione:
 - Il progetto del ponte è concepito con l’obiettivo di rendere più agevole il transito di imbarcazioni di simili dimensioni, 
   avendo come focus principale il parametro dell’altezza. Per evitare complicazioni correlate alle grandi differenze di larghezza 
   fra le imbarcazioni, in tale progetto non è previsto il transito di navi/imbarcazioni di grossa stazza.
 - Nel caso in cui molteplici imbarcazioni passassero simultaneamente davanti al primo sensore ad ultrasuoni, si potrebbe 
   verificare una conta inaccurata, con la conseguente compromissione della funzione della regolazione del flusso. 
   Tuttavia, quest’eventualità non è contemplata, in quanto, tra la sponda in cui è posto il sensore ad ultrasuoni e l’altra, è 
   posizionato uno scoglio con lo scopo di rendere la zona di attesa accessibile ad un’unica imbarcazione per volta. 
   Lo stesso procedimento viene applicato anche per la zona di uscita. 
 - Il flusso del traffico, sia marittimo che stradale, è unidirezionale. In quanto il focus del progetto è l'utilizzo del ponte 
   unicamente in fase di entrata nella zona portuale.

## Limiti progettuali: 
 - A seguito della rilevazione della prima imbarcazione, si avvia un timer di 60 secondi, terminato tale tempo la sbarra dirigi 
   traffico si chiuderà e si aprirà il ponte. 
   In quest’intervallo, le condizioni del traffico marittimo sono variabili: la quantità di imbarcazioni presenti nella zona d'attesa allo 
   scadere del timer, determina il lasso di tempo in cui il ponte rimarrà aperto. 
   Se durante il processo di apertura arrivassero ulteriori imbarcazioni con l’intenzione di oltrepassare il ponte, quest’ultime 
   dovranno attendere prima del sensore di rilevazione d’entrata.
 - Nel progetto non vengono utilizzati sistemi di controllo per verificare che, durante i processi, non siano presenti veicoli al 
   di sopra o imbarcazioni al di sotto.
