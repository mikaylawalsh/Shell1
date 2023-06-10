Structure/Compiling:
The structure of our program has a few main parts. The first part is the parser 
which parses the initial input (in the buffer) into many different arrays that 
we use later but most importantly the argv array (that is read from for most of 
command line handleing), our filepath array which extracts the file path from 
our argv array to be used for system calls, and lastly our redirection array 
which is used for redirection.

The next part is where we handle built-in shell commands. This is made up of 
helpers that all call the necessary C function system calls to execute a given 
command. 

Our next step handles redirection and creates a child process. This 
function checks for the occurances of a redirection symbol and does the 
necessary operations if there is an occurance. This is done through opening and 
closing system calls with the necessary flags. 

We then put all of these helpers together by simply checking for an occurance 
of a built-in and then handling redirection from there if there are not builtins 
included. These helpers also account for the basic cases (entering excess space 
or entering nothing, etc.)

Our main functin creates a repl that first checks if the PROMPT is set. If it is
we dipslay the prompt. We then read in from the stdin and put it in a buffer. 
If the read is successful, then the main calls parse on the buffer and empty 
arrays. Memset is used to fill it with 0's or NULLs initially. If parse is 
successful, then we call built_in which will then run all of the necessary 
helper functions. When this is finished, the user is reprompted and the 
process repeates until the CTRL-D or exit is entered. 

We also error check for both system calls and library functions as well as error 
checking shell commands that should fail. 

To compile our program, the user types in make clean all. It should compile with
no warnings. We then run either ./33sh for prompt or 33noprompt for no prompt 
to enter into our shell. 

Bugs and Extra Feature: N/A

Work distribution:
We mostly worked on our code together. We discussed the concepts and then tried 
to write the majority of the code together. On the limited occasions where we 
worked seperately, we would meet back up and make sure that both partners 
understood the changes made both conceptually and code wise. We both worked on 
debugging together as well.
