

/*
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

*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <ctype.h>

#include "tiffDrawingTools.h"

#define PI 3.14159265359

#define WID 2000
#define HWID 1000

typedef struct {
    char draw[26];
    char skip[26];
    char *axiom;
    char *rules[26];
    double angle;
    double startAngle;
    double currentAngle;
    int iterations;
    double pos[2];
} lsystem;

void generate(screen *sp, lsystem *ls, char *current, int iteration, int drawable, double extremes[5]);

void lSystemInit(lsystem *ls);

int beginsWith(char *, char *);

int main(int argc, char **argv) {
    lsystem s1;
    char *tp;
    lSystemInit(&s1);
    for(int i = 1; i < argc; ++i) { /* get command line arguments */
        if(beginsWith("draw=",argv[i]) || beginsWith("skip=",argv[i])) {
            tp = beginsWith("draw=",argv[i]) ? s1.draw : s1.skip;
            for(int a = 5; argv[i][a] != 0; ++a) {
                if(islower(argv[i][a]))
                    tp[argv[i][a] - 'a'] = 1;
                else {
                    printf("Please only use lower case letters\n.");
                    return 0;
                }
            }
        }
        else if(beginsWith("axiom=",argv[i]))
            s1.axiom = argv[i] + 6;
        else if(beginsWith("iterations=",argv[i]))
            s1.iterations = atoi(argv[i] + 11);
        else if(beginsWith("angle=",argv[i]))
            s1.angle = atof(argv[i] + 6) / 180.0 * PI;
        else if(beginsWith("startingAngle=",argv[i]))
            s1.startAngle = s1.currentAngle = atof(argv[i] + 14) / 180.0 * PI;

        else if(strlen(argv[i]) >= 3 && argv[i][1] == '=') {
            if(!islower(argv[i][0])) {
                printf("Please only use lower case letter rules");
                return 0;
            }
            else
                s1.rules[argv[i][0] - 'a'] = argv[i] + 2;
        }
        else {
            printf("Unidentified argument `%s'\n", argv[i]);
            return 0;
        }
    }
    if(!s1.axiom) {
        printf("Please provide a starting axiom with 'axiom='\n");
        return 0;
    }

    double extremes[4] = {0,0,0,0}; // extremes will be set during the first call of generate. These are used to center and scale the image to fill the picture
    double tmp;
    
    screen *sp = sopen(WID,WID); // create image object

    generate(sp, &s1, s1.axiom, s1.iterations, 0, extremes); // set the extremes, do not draw
    tmp = extremes[2];
    // convert the extremes into centerpoint and frame width. this data is used by the second call to generate, which actually draws the image to the screen
    extremes[2] = ((extremes[1] - extremes[0]) > (extremes[3] - extremes[2])) ? (extremes[1] - extremes[0])/2.0 * 1.2: (extremes[3] - extremes[2])/2.0 * 1.2;
    extremes[0] = (extremes[0] + extremes[1]) / 2.0;
    extremes[1] = (tmp + extremes[3]) / 2.0;
    s1.pos[0] = s1.pos[1] = 0;
    s1.currentAngle = s1.startAngle;
    // draw image.
    generate(sp, &s1, s1.axiom, s1.iterations, 1, extremes);
    writeFile("LSystem.tiff",sp);
    sclose(sp);

}



void generate(screen *sp, lsystem *ls, char *current, int iteration, int drawable, double extremes[4]) {
    double brackets[8][3];
    int goTo = -1;
    double tmp[2];
    double coeff;

    char c;
    int a;
    for(int i = 0; (c = current[i]); ++i) {
        if(islower(c)) {
            a = c-'a';
            if(iteration == 0 || !(ls->rules[a])) { // last iteration or there is no rule for that letter
                if(ls->draw[a]) {
                    tmp[0] = ls->pos[0];
                    tmp[1] = ls->pos[1];
                    // move pen
                    ls->pos[0] += cos(ls->currentAngle);
                    ls->pos[1] += sin(ls->currentAngle);
                    if(drawable) {
                        coeff = HWID / extremes[2];
                        drawLine2d(sp, ((tmp[0] - extremes[0]) * coeff + HWID), ((tmp[1] - extremes[1]) * coeff + HWID), ((ls->pos[0] - extremes[0]) * coeff + HWID), ((ls->pos[1] - extremes[1]) * coeff + HWID));
                    }
                }
                else if(ls->skip[a]) {
                    ls->pos[0] += cos(ls->currentAngle);
                    ls->pos[1] += sin(ls->currentAngle);
                }
                if(!drawable) { // if drawable is not set, then the function finds the extremes (where the outer most points of the L-system are)
                    if(ls->pos[0] < extremes[0])
                        extremes[0] = ls->pos[0];
                    else if(ls->pos[0] > extremes[1])
                        extremes[1] = ls->pos[0];
                    if(ls->pos[1] < extremes[2])
                        extremes[2] = ls->pos[1];
                    else if(ls->pos[1] > extremes[3])
                        extremes[3] = ls->pos[1];
                }
            }
            else if(ls->rules[a][0])
                generate(sp, ls, ls->rules[a], iteration-1, drawable, extremes);

        }
        else if(c == '+')
            ls->currentAngle += ls->angle;
        else if(c == '-')
            ls->currentAngle -= ls->angle;
        else if(c == '[') {
            brackets[++goTo][0] = ls->pos[0];
            brackets[goTo][1] = ls->pos[1];
            brackets[goTo][2] = ls->currentAngle;
        }
        else if(c == ']') {
            ls->pos[0] = brackets[goTo][0];
            ls->pos[1] = brackets[goTo][1];
            ls->currentAngle = brackets[goTo--][2];
        }
    }
}

void lSystemInit(lsystem *ls) {
    memset(ls->draw,0,26);
    memset(ls->skip,0,26);
    memset(ls->rules,0,26*sizeof(char *));
    ls->axiom = NULL;
    ls->angle = PI / 3.0;
    ls->startAngle = ls->currentAngle = 0;
    ls->iterations = 1;
    ls->pos[0] = ls->pos[1] = 0;
}

int beginsWith(char *begin, char *string) {
    for(int i = 0; begin[i] != 0; ++i) {
        if(begin[i] != string[i] || !string[i])
            return 0;
    }
    return 1;
}
