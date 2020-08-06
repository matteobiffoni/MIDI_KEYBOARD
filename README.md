# MIDI_KEYBOARD
Progetto di Matteo Biffoni per l'esame di Sistemi Operativi, Università degli Studi di Roma \"La Sapienza\".

Il progetto si propone di realizzare una tastiera musicale di tipo **MIDI**.
Sono presenti 3 programmi \"principali\":\
\
&nbsp;&nbsp;&nbsp;&nbsp;**1**.  Sul microcontrollore viene caricato un programma che si occupa di trasferire i dati ricevuti dai pulsanti (tasti) al computer\
&nbsp;&nbsp;&nbsp;&nbsp;**2**.  Sul computer un programma si occupa di gestire i dati che provengono dal microcontrollore, di costruire un file MIDI (.mid) tramite questi
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;e infine di salvare il file\
&nbsp;&nbsp;&nbsp;&nbsp;**3**.  Un terzo programma (sul computer) si occupa di riprodurre i file registrati con la tastiera
