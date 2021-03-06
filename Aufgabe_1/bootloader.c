/* needs to stay the first line */
asm(".code16gcc\njmp $0, $main");

#include <windows.h>
#include <gtk/gtk.h>

typedef int bool;
#define true 1
#define false 0

char newLine[]="\n";

/*functions*/
void print_c(char ch){                   /*print character to screen*/
    asm("mov $0x0e, %%ah;"               /*loading function 0eh (print character) into register AH*/
        "mov %0, %%al;"                  /*saving  content of %0 into register AL*/
        "int $0x10" :: "r"(ch));         /*interrupt 10h (video services and r = chatacter to print)*/
}

void print_s(char msg[],  int j){        /*print string to screen*/
    int i=0;                             /*interrupt variable for while loop*/
    while(i <=j){                        /*repeat as many time as there are characters im the string*/
        print_c(msg[i]);                 /*print character function with character to print*/
        i=i+1;                           /*interrupt variable +1*/
    }
    print_c('\r');
}

void reboot(){                           /*reboots the system*/
    asm("int $0x19");                    /*loads the os after POST*/
}

void print_p(){                          /*prints . to hide the password*/
    print_c(46);                         /*ASCII code for . */
}

char get_input(){                        /*get Keyboard input*/
    char c;                              /*variable to save keyboard input*/
    asm volatile(
        "mov %%ah, 1;"                   /*loading function 01h into register AH*/
        "int $0x16;"                     /*interrupt 16h (keyboard services)*/
        :"=r"(c)                         /*saving the pressed key(ASCII code) into c*/
        :
        :
    );
    return c;                            /*returns pressed key(ASCII code)*/
}

int hidden_p_input(){                    /*only works with return type int for some reason (every enter reboots?!)*/
    char pass[8];
    bool ent = true;

    for (int i=0; i<=7; i++){
        char tmp = get_input();
        if(i==0 && tmp==13){             /*empty string  and enter*/
            reboot();
        }
        pass[i]=tmp;
        if(tmp==13){                    /*shorter password than max size*/
            print_s(newLine, 1);
            print_s(pass, i);
            return 1;
        }
        pass[i]=tmp;
        print_p();                      /*prints . for the password*/
    }

    while(ent){                         /*max pw print to screen only when pressing enter*/
        char tmp1 = get_input();
        print_c(32);                   /*akzeptiert ohne diese ausgabe kein enter (CR) nach eingabe eines anderen buchstabens*/
        print_c(8);                    /*Backspace to cover  up additional chracters while waiting for enter key*/
        if(tmp1==13){
            print_s(newLine, 1);
            print_s(pass, 7);
            ent = false;
        }
    }  

    return 0;
}

void os(){
	bool run = true;

	print_s("Hello!\n",8);

	while(run){
		hidden_p_input();
        print_s(newLine, 1);
	}
}

void calc(){

}

void main(void){
	asm(
		"mov $0x007, %%ebx;"
		::: "eax", "ebx"
	);

	os();

	asm(
		"jmp .;"
	);
}