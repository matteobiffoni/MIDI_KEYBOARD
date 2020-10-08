# Note player simple example

This file tests the audio mechanism used for this project, such as OpenAL and its components.

In order to execute the test, follow these instructions:

* Open a terminal
* Run ```sudo apt-get install libopenal1```
* Run ```sudo apt-get install libopenal-dev```
* Run ```make```

Now, if everything was executed successfully, the test can be executed by running in the terminal ```./note_player_simple_test```

### Test features

Once the program is being executed, there are several actions that can be performed:

* Insert a note name (character: '```c```', '```d```', '```e```', '```f```', '```g```', '```a```', '```b```') and press ```Enter``` to enable that note (insert again to disable)
* Chords are supported, so multiple notes can be enabled simultaneously
* To quit the test, insert another character

Have fun!
