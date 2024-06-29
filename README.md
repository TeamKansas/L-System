This is an L-System generator that takes its input from command line arguments.
What is an L-System?
An L-System is a set of lines or shapes created using an explicitly defined set of rules and inputs, which are provided by the user in this program.
The first input that it needs are the symbols that represent lines. In this program these symbols must be lower case letters. Set with `draw='
The user may also provide symbols for the program to represent as invisible lines, also all lower case. Set with `skip='
The next input it needs is the starting axiom. This is the seed that the program will start drawing with. Set with `axiom='
The program also needs the number of recursions to perform. Set with `iterations='
The program needs the angle that the pen will turn every time it comes across a `+' or `-'. Set with `angle='
Finally, the user may provide extra rules that define what the program will do during recursion. set with `<Letter>='
When the program comes across a letter that is defined by such a rule AND it is not on it's last iteration, then it will draw the pattern associated with that letter.
If it is on it's last iteration, it will draw based on the rules given by the skip= and draw= arguments.

make sure that none of your arguments have any spaces ` ' in them unless you surround the entire argument in double quotation marks `"'. 
Otherwise, the program will separate your arguments into multiple arguments, and it will not understand them.
To set a specific input, you type the name of the input (eg. iterations) followed immediately by an equals `=' sign, followed immediately by the value you wish to provide.
For example if you want to set the number of recursions to five, the command line argument would be `iterations=5'
the allowed inputs are as follows:
draw ............ the letters you wish the program to represent as drawn (visible) lines
skip ............ the letters you wish the program to represent as blank (invisible) lines
axiom ........... the starting axiom that the program begins drawing with.
iterations ...... the number of recursions the program will perform before actually drawing the letters
angle ........... the value in degrees that the drawing direction will change on `+' or `-'
startingAngle ... the value in degrees of the starting direction of the pen

To set a new rule, first type the letter you would like to set the rule for, immediately followed by an equals `=' sign, immediately followed by the rule itself
for example if you would like the letter a to be recursed as 'a+a--a+a' you would provide the argument
a=a+a--a+a

there are 26 available rule slots, one for each letter. 

grammar for rules and axiom:
letter: recursed or drawn depending on the rules you give
'-' or `+': increase or decrease the pen angle by the increment angle. this angle is definded by `angle='
Placing characters inside brackets will cause the program to resume the place it left off once it leaves the brackets

the output image of this program is placed into a .tiff image file called LSystem.tiff

Examples: 
Tree: ./LSystem angle=30 draw=a axiom=a a=a[+a][-a]a iterations=4
Koch curve: ./LSystem angle=60 draw=a axiom=a a=a+a--a+a iterations=5
full Koch snowflake: ./LSystem angle=60 draw=a axiom=a--a--a a=a+a--a+a iterations=5
