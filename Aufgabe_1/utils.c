#include "utils.h"

char newLine[]="\n";

/*functions*/
void print_C(char ch){                   /*print character to screen*/
    asm("mov $0x0e, %%ah;"               /*loading function 0eh (print character) into register AH*/
        "mov %0, %%al;"                  /*saving  content of %0 into register AL*/
        "int $0x10" :: "r"(ch));         /*interrupt 10h (video services and r = chatacter to print)*/
}

void print_S(char msg[],  int j){        /*print string to screen*/
    int i=0;                             /*interrupt variable for while loop*/
    while(i <=j){                        /*repeat as many time as there are characters im the string*/
        print_C(msg[i]);                 /*print character function with character to print*/
        i=i+1;                           /*interrupt variable +1*/
    }
    print_C('\r');
}

void reboot(){                           /*reboots the system*/
    asm("int $0x19");                    /*loads the os after POST*/
}

void print_P(){                          /*prints . to hide the password*/
    print_C(46);                         /*ASCII code for . */
}

char get_input(){                        /*get Keyboard input*/                                 /*wartet evt net auf user input*/
    char c;                              /*variable to save keyboard input*/
    asm (
        "mov %%ah, 1;"                   /*loading function 01h into register AH*/
        "int $0x16;"                     /*interrupt 16h (keyboard services)*/
        :"=r"(c)                         /*saving the pressed key(ASCII code) into c*/
        :
        :
    );
    return c;                            /*returns pressed key(ASCII code)*/
}

int hidden_p_input(){                    /*only works with return type int for some reason (every enter reboots?!)*/
    char pass[9];
    for (int i=0; i<=8; i++){
        char tmp = get_input();
        if(i==0 && tmp==13){
            reboot();                   /*empty string  and enter*/
        }
        pass[i]=tmp;
        if(tmp==13){                    /*shorter password than max size*/
            print_S(newLine, 2);
            print_S(pass, 8);
            return 1;
        }
        pass[i]=tmp;
        print_p();                      /*prints . for the password*/
    }
    print_s(newLine, 2);
    print_S(pass, 8);
    return 0;
}