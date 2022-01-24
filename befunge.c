#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <ctype.h>
#include <string.h>

#define SLEEPDELAYMS 1
#define ANIMATIONMODE 1
#define MAXITERATION 1000
#define STACKSIZE 1000


void printCode(const char *code, const int size, const int width, const int pos, const int *stack, const int stackPtr);
void char2emoji(const char c);
void printHelp(const char *execName);


/**
 * @brief Reads befunge code from a file or defaults to a factorial program. Translates befunge code to emojis
 */
int main(int argc, char *argv[]){
    char *program;              // Program string (can be self modifying, so programs can never be const)
    int width, height;          // Width and height of program string

    int x=0, y=0, i=0;          // x,y represents the program counter
    char c;                     // c represents value at program pointer position
    bool stringmode = false;    // Stringmode is true when reading a string literal

    int stack[STACKSIZE] = {0};
    int stackPtr = 0;

    enum {RIGHT, LEFT, DOWN, UP} direction = RIGHT;
    int directions[4][2] = {{1,0},{-1,0},{0,1},{0,-1}};


    // Read program from file if number of arguments are 4
    // Default to factorial program with no arguments
    // Print help text if not 0 or 4 arguments
    if(argc == 4){
        FILE *f = fopen(argv[1], "r");

        width = atoi(argv[2]);
        height = atoi(argv[3]);

        if(width && height && f){
            fseek (f, 0, SEEK_END);
            int length = ftell(f);
            fseek (f, 0, SEEK_SET);
            program = malloc(length);

            if (program) {
                fread(program, 1, length, f);
            }

            fclose (f);
        }else{
            printHelp(argv[0]);
            return 0;
        }
    }else{
        // Factorial
        char factorial[] = 
            "&>:1-:v v *_$.@"
            " ^    _$>\\:^   ";
        // Width and height of the program string
        width = 15;
        height = 2; 
        program = factorial;
    }

    printf("\x1b[2J \033[0;0H"); // Clear screen and move cursor to 0,0

    // Transpile char at program counter
    while(i++<MAXITERATION){
        // Read char pointed to
        c = program[x+y*width];

        if(stringmode && c != 'z' && c != '"'){ // If stringmode:  Push c to stack as ASCII
            stack[++stackPtr] = c;
        }else if(c>='0' && c<='9'){             // If 0-9:         Push numbers on the stack
            stack[++stackPtr] = c-'0';
        }else{                                  // Else:           Interpret c as an operation
            switch(c){                              
                // +	Addition: Pop a and b, then push a+b
                case '+':
                    {
                        int a = stack[stackPtr--];
                        int b = stack[stackPtr--];
                        stack[++stackPtr] = a+b;
                    }
                break;

                // -	Subtraction: Pop a and b, then push b-a
                case '-':
                    {
                        int a = stack[stackPtr--];
                        int b = stack[stackPtr--];
                        stack[++stackPtr] = b-a;
                    }
                break;
                
                // *	Multiplication: Pop a and b, then push a*b
                case '*':
                    {
                        int a = stack[stackPtr--];
                        int b = stack[stackPtr--];
                        stack[++stackPtr] = a*b;
                    }
                break;
                
                // /	Integer division: Pop a and b, then push b/a, rounded towards 0.
                case '/':
                    {
                        int a = stack[stackPtr--];
                        int b = stack[stackPtr--];
                        stack[++stackPtr] = b/a;
                    }
                break;

                // %	Modulo: Pop a and b, then push the remainder of the integer division of b/a.
                case '%':
                    {
                        int a = stack[stackPtr--];
                        int b = stack[stackPtr--];
                        stack[++stackPtr] = a%b;
                    }
                break;

                // !	Logical NOT: Pop a value. If the value is zero, push 1; otherwise, push zero.
                case '!':   stack[stackPtr] = !stack[stackPtr];                             break;

                // ` (backtick) Greater than: Pop a and b, then push 1 if b>a, otherwise zero.
                case '`':
                    {
                        int a = stack[--stackPtr];
                        int b = stack[--stackPtr];
                        stack[++stackPtr] = b>a;
                    }
                break;

                // >	Start moving right
                case '>':   direction = RIGHT;                                              break;

                // <	Start moving left
                case '<':   direction = LEFT;                                               break;

                // ^	Start moving up
                case '^':   direction = UP;                                                 break;

                // v	Start moving down
                case 'v':   direction = DOWN;                                               break;

                // ?	Start moving in a random cardinal direction
                case '?':   direction = rand()%4;                                           break;

                // _	Pop a value; move right if value=0, left otherwise
                case '_':   direction = stack[stackPtr--]?LEFT:RIGHT;                       break;

                // |	Pop a value; move down if value=0, up otherwise
                case '|':   direction = stack[stackPtr--]?UP:DOWN;                          break;

                // "	Start string mode: push each character's ASCII value all the way up to the next "
                case 'z':
                case '"':   stringmode = !stringmode;                                       break;

                // :	Duplicate value on top of the stack
                case ':':
                    {
                        stackPtr++;
                        stack[stackPtr] = stack[stackPtr-1];
                    }
                break;

                // $	Pop value from the stack and discard it
                case '$':   stackPtr--;                                                     break;

                // .	Pop value and output as an integer followed by a space
                case '.':   printf("%i ", stack[stackPtr--]);                               break;

                // ,	Pop value and output as ASCII character
                case ',':   putchar(stack[stackPtr--]);                                     break;

                // #	Bridge: Skip next cell
                case '#':   x += directions[direction][0] % width;                          break;

                // p	A "put" call (a way to store a value for later use). Pop a, b, and c, then change the character at (a,b) in the program to the character with ASCII value c
                case 'p': 
                    {
                        int a = stack[stackPtr--];
                        int b = stack[stackPtr--];
                        int c = stack[stackPtr--];
                        program[a+b*width] = c;
                    }
                break;

                // g	A "get" call (a way to retrieve data in storage). Pop y and x, then push ASCII value of the character at that position in the program
                case 'g': 
                    {
                        int a = stack[stackPtr--];
                        int b = stack[stackPtr--];
                        stack[++stackPtr] = program[a+b*width];
                    }
                break;

                // &	Ask user for a number and push it
                case '&':   scanf(" %i", &stack[++stackPtr]);                               break;
                
                // ~	Ask user for a character and push its ASCII value
                case '~':
                    {
                        char input;
                        scanf(" %c", &input);
                        stack[++stackPtr] = (int)input;
                    }
                break;

                // @	End program
                case '@':   goto end;                                                       break;

                // (space)	No-op. Does nothing
                case ' ':   break;

                // \	Swap two values on top of the stack
                case '\\': 
                    {
                        int temp = stack[stackPtr];
                        stack[stackPtr] = stack[stackPtr-1];
                        stack[stackPtr-1] = temp;
                    }
                break;

                // Invalid character
                default:
                    printf("\n invalid char %i", c);
                break;
            }
        }

        // Step to next character
        x += directions[direction][0];
        y += directions[direction][1];

        // Wrap around if hitting wall
        if(x<0) x = width-1;
        if(y<0) y = height-1;
        x %= width;
        y %= height;

        // Return 0 when attempting to pop empty stack
        if(stackPtr<0){
            stack[0] = 0;
            stackPtr = 0;
        }

        // Print code, stack, and highlight char pointed to
        printCode(program, width*height+1, width, x+y*width, stack, stackPtr);
    }

    end:
    printf("\033[15;0H \033[J");
    printCode(program, width*height+1, width, x+y*width, stack, stackPtr);
    return 0;
}


/**
 * @brief Prints a short usage text for the executable
 * 
 * @param execName - name of the executable (from argv[0])
 */
void printHelp(const char *execName){
    printf("Usage: %s <file> <width> <height>", execName);
}



/**
 * @brief Displays program and highlights char at program counter. Also displays stack as int and char
 * 
 * @param code      - program code
 * @param size      - length of program code
 * @param width     - width of program code
 * @param pos       - program counter position
 * @param stack     - pointer to stack array
 * @param stackPtr  - length of stack array
 */
void printCode(const char *code, const int size, const int width, const int pos, const int *stack, const int stackPtr){

    // Save cursor position:
    printf("\033[s");

    // Move cursor to (8, 0), clear from here to rest of screen
    printf("\033[8;0H \033[J");

    // Print stack (as ASCII and as int) and stackPtr value
    printf("\nstack (char): [");
    for(int i=0; i<stackPtr+1; i++){
        if( stack[i] > 10 && stack[i] < 255){
            printf("%c", stack[i]);
        }else{
            printf("(%i)", stack[i]);
        }
    }
    printf("]\nstack (int): [");
    for(int i=0; i<stackPtr+1; i++){
        printf(" %i,", stack[i]);
    }
    putchar(']');

    // Print all chars up to <pos> normally
    for(int i=0; i<pos; i++){
        if(i%width == 0) putchar('\n');

        char2emoji(code[i]);
    }

    // Highlight character at <pos>
    if(pos%width == 0) putchar('\n');
    printf("\033[43m");
    char2emoji(code[pos]);
    printf("\033[0m");


    // Print the remaining characters normally
    for(int i=pos+1; i<size; i++){
        if(i%width == 0) putchar('\n');

        char2emoji(code[i]);
    }

#if ANIMATIONMODE
    usleep(SLEEPDELAYMS * 1000);
#else
    getchar();
#endif

    //- Restore cursor position:
    printf("\033[u");
}



/**
 * @brief Prints emoji representation of program character
 * 
 * @param c program character
 */
void char2emoji(const char c){
#if 1
    switch(c){
        case '0': printf("0️⃣ "); break;
        case '1': printf("1️⃣ "); break;
        case '2': printf("2️⃣ "); break;
        case '3': printf("3️⃣ "); break;
        case '4': printf("4️⃣ "); break;
        case '5': printf("5️⃣ "); break;
        case '6': printf("6️⃣ "); break;
        case '7': printf("7️⃣ "); break;
        case '8': printf("8️⃣ "); break;
        case '9': printf("9️⃣ "); break;
        case '<': printf("👈"); break;
        case '>': printf("👉"); break;
        case 'v': printf("👇"); break;
        case '^': printf("👆"); break;
        case '+': printf("➕"); break;
        case '-': printf("➖"); break;
        case '*': printf("✖️ "); break;
        case '/': printf("➗"); break;
        case '%': printf("🚮"); break;
        case '!': printf("🚫"); break;
        case '`': printf("📏"); break;
        case '?': printf("🎲"); break;
        case '_': printf("🔛"); break;
        case '|': printf("🪜"); break;
        case '"': printf("📜"); break;
        case ':': printf("🧬"); break;
        case '\\':printf("💱"); break;
        case '$': printf("🗑️ "); break;
        case '.': printf("🔢"); break;
        case ',': printf("✍"); break;
        case '#': printf("🦘"); break;
        case 'p': printf("📥"); break;
        case 'g': printf("📤"); break;
        case '&': printf("📲"); break;
        case '~': printf("🔤"); break;
        case '@': printf("🏁"); break;
        case ' ': printf("  "); break;
    }
#else
    putchar(c);
#endif
}