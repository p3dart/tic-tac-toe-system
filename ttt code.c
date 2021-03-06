/*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
    File Name:  ELNC6007265Lab4.c
    Author:     PARTH LAD
    Date:       11/25/2020
    Modified:   Name or None
    

    Description: *
* THUS IS GAME OF TIC TAC TOE WHERE TWO PLAYER CAN PLAY THE GAME AND DECLARE THE WINNER BY USER INPUT
* ALSO CONTROLL THE MULTIPLE LEDS ACCORDING TO USER INPUT FROM THE PUSH BUTTON
* THIS PROGRAM ALSO DISPLAY THE TIME 
* AND ALSO RESET THE GAME IF MODE AND ENTER IS PRESSED FOR 3 SECONDS
* ALSO DIPLAY ONE STRING FOR 5 SECONDS AND COUNT DOWN DISPLAY.
* IT SEND THE COMMUNICATION SENTENCE TO CONTROL AND ALSO RECEIVING COMMUNICATION SENTENTCE FROM CONTROLLER AND EXCUATE IT.  
    
>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/
//==========================HEADER FILE======================================================
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include "mbed.h"

//=============================CONSTANTS=====================================================
#define TRUE            1
#define FALSE           0
#define SEC             0
#define MIN             1
#define BUFSIZE         35
#define TOKENSIZE       35
#define TTTRECEIVE      1
#define TTTSEND         265

//=============================Golbal variables=============================================
char row=0;             //variable to hold the value of row
char column=0;          // variable to hold the value of column
char type=0;            // variable to secelct row=0 or column=1 
char index1=FALSE;      // variable for initialization loop
char index2=FALSE;      // variable for initialization loop
char rxbuf1[BUFSIZE];
char txbuf1[BUFSIZE];
char txReset[BUFSIZE];
char *tokens[BUFSIZE];
char *ptr=rxbuf1;
char sentenceReady=FALSE;
char insert = 0;
char plypmt[]={"PLYPMT"};
char timerReset=0;
char timerResetFlag=0;

Ticker t;
DigitalIn PB1(p18);
DigitalIn PB2(p17);
DigitalIn PB3(p16);
DigitalIn PB4(p15);

Serial pc(USBTX, USBRX); // USB serial channel to host PC
Serial pic265( p28, p27);
typedef struct gameBoard
{
    char TicTacToe[3][3];                //declare 2d array for the game board
    char player;                        // declare the variable to indicate which player is active
    int turnCounter;                     // variable to count the how many turns have been taken 
    int winFlag;                        // true flase variable tht indicate the a win cordinate       
}game265_t;

typedef struct TicTacToeSystem
{
    int address;                          //variable to hold the student id number 
    game265_t game;
    int select;                           // variable that select between row and column
    char time[2];                           // variable array that holds minuits and second
    char status;                           //indicator varible which display current ststus of game
    char turnTime;
}ttt_t;

ttt_t ttt265;

//==============================================FUNCTIONS=====================================================================
/*>>> initialTtt >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
Author:     pl
Date:       02/10/2020
Modified:   Name or None
Desc:       it will initialize the variable of the global data structure
Input:      none
Returns:    none
>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/
void initialTtt(void)
{
    //single variable
    row=0;
    column=0;
    type=0;
    ttt265.address=0;
    ttt265.game.player=FALSE;
    ttt265.game.turnCounter=FALSE;
    ttt265.game.winFlag=FALSE;
    ttt265.select=FALSE;
    ttt265.turnTime=30;
    //multiple variable /array

    //use loop to initialize whole array and set the value at 0xff
    for(index1=0;index1<3;index1++)
    {
        for(index2=0;index2<3;index2++)
        {
            ttt265.game.TicTacToe[index1][index2]=0XFF;
        }
    }
    ttt265.time[SEC]=FALSE;
    ttt265.time[MIN]=FALSE;
}// eo initialTtt::*********************************************************************************************************
/*>>> checkSum >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
Author:     pl
Date:       04/11/2020
Modified:   Name or None
Desc:       this function will return the value of checksum in which i have use XOR logic to check the checksum for string
Input:      input always a pointer to start with
Returns:    it will return char value of checksum 
>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/
char checkSum(char *ptr)
{
    char cs=0;
    while(*ptr)
    {
        cs^=*ptr;
        ptr++;
    }
    return cs;
}
//EO: CHECKSUM********************************************************************************
/*>>> playerReturn >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
Author:     Name
Date:       05/10/2020
Modified:   Name or None
Desc:       this function will determine according to player name either X or O from turn count
Input:      none
Returns:    none
>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/
char playerReturn()
{
    if(ttt265.game.turnCounter==0 ||ttt265.game.turnCounter==2 ||ttt265.game.turnCounter==4||ttt265.game.turnCounter==6||ttt265.game.turnCounter==8)
    {
        ttt265.game.player='O';
        return 'O';
    }
    else
    {
        ttt265.game.player='X';
        return 'X';
    }
}// eo  playerReturn::*******************************************************************************
/*>>> stringPrint1 >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
Author:     pl
Date:       04/11/2020
Modified:   Name or None
Desc:       this function will create a communication sentence while there is any auto placement is done 
Input:      none
Returns:    none
>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/
void stringPrint1()
{
        sprintf(txbuf1,"$,PLYPMT,%i,%i,%i,%i,%c",TTTRECEIVE,TTTSEND,row,column,playerReturn());
        sprintf(txbuf1,"%s,%i",txbuf1,checkSum(txbuf1));
        sprintf(txbuf1,"%s^",txbuf1);
        pc.printf("%s",txbuf1);
        pic265.printf(txbuf1,"%s",txbuf1);
}
//eo:stringPrint**************************************************************************************

/*>>> resetString >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
Author:     pl
Date:       04/12/2020
Modified:   Name or None
Desc:       this function will create a communication sentence while there is any auto placement is done or
Input:      none
Returns:    none
>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/
void resetString()
{
    sprintf(txReset,"$,GAMRST,%i,%i,\0",TTTRECEIVE,TTTSEND);
    sprintf(txReset,"%s,%i^",txReset,checkSum(txReset));
    pc.printf("%s",txReset);
    pic265.printf(txReset,"%s",txReset);
}
//eo:: resetString**********************************************************************************************
/*>>> increment  >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
Author:     pl
Date:       02/10/2020
Modified:   Name or None
Desc:       increment the value of respective row and column value
Input:      when the increment pushbutton pressed this function will excute
Returns:    it will return the value of incremented value of row or column
>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/
void increment()
{
    // condition for row increment
    if(type==0)
    {
        ttt265.select++;
        if(ttt265.select>2)
        {
            ttt265.select=FALSE;
        }
        row=ttt265.select;  
    }
    // condition for column increment
    else if(type==1)
    {
        ttt265.select++;
        if(ttt265.select>2)
        {
            ttt265.select=FALSE;
        }
        column=ttt265.select;   
    }   
}// eo increment::*******************************************************************************
/*>>> decrement  >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
Author:     pl
Date:       02/10/2020
Modified:   Name or None
Desc:       decrement the value of respective row and column value
Input:      when the decrement pushbutton pressed this function will excute
Returns:    it will return the value of decremented value of row or column
>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/
void decrement()
{
    //condition for row decrement
    if(type==0)
    {
        ttt265.select--;
        if(ttt265.select<0)
        {
            ttt265.select=2;
        }
        row=ttt265.select;  
    }
    ////condition for column decrement
    else if(type==1)
    {
        ttt265.select--;
        if(ttt265.select<0)
        {
            ttt265.select=2;
        }
        column=ttt265.select;   
    }
}// eo decrement::*******************************************************************************
/*>>> pressEnter >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
Author:     pl
Date:       02/10/2020
Modified:   Name or None
Desc:       this function is help to place the value of x or o to selscted place
Input:      none
Returns:    none
>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/
void pressEnter(void)
{
    // first condition to change the variable row to coulmn
    if(type==0)
    {
        type=1;
    }
    // condition to place the character
    else if(type==1)
    {
        if(ttt265.game.TicTacToe[row][column]!='X' && ttt265.game.TicTacToe[row][column]!='O')
        {
            if(ttt265.game.turnCounter==0 ||ttt265.game.turnCounter==2 ||ttt265.game.turnCounter==4||ttt265.game.turnCounter==6||ttt265.game.turnCounter==8)
            {
                ttt265.game.TicTacToe[row][column]='X';
            }
            else
            {
                ttt265.game.TicTacToe[row][column]='O';
            }
            type=FALSE;
            ttt265.turnTime=30;
            stringPrint1();
            ttt265.game.turnCounter++;     //turn count increment                
        }
        else
        {
            type=0; 
        }
    }
}// eo pressEnter ::************************************************************************************************88
/*>>> userInput  >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
Author:     pl
Date:       06/12/2020
Modified:   Name or None
Desc:       take the input frrom user and mask switch and take action accordingly
Input:      when the increment pushbutton pressed this function will excute
Returns:    it will return the value of incremented value of row or column
>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/
void userInput()
{
    if(PB1==FALSE)
    {
        increment();
    }
    else if(PB2==FALSE)
    {
        decrement();
    }
    else if(PB3==FALSE)
    {
        pressEnter();   
    }
    if(PB4==FALSE && PB3==FALSE)
    {
        timerResetFlag=TRUE;
        if(timerReset>=3)
        {
            resetString();
            initialTtt();    
        }
     }    
}// eo userinput::*******************************************************************************
/*>>> winCheck >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
Author:     pl
Date:       03/10/2020
Modified:   Name or None
Desc:       it will check the all winning condition for the player1 and player2  
Input:      it will take input as all the game board array menmber and check the winning condition after the turn count goes 5
Returns:    if the any of the winning condition is true the function will return the value according to winning condition
>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/
void winCheck()
{
    if(ttt265.game.TicTacToe[0][0]=='X'&&ttt265.game.TicTacToe[0][1]=='X'&&ttt265.game.TicTacToe[0][2]=='X')
    {
        ttt265.game.winFlag=TRUE;
    }
    if(ttt265.game.TicTacToe[1][0]=='X'&&ttt265.game.TicTacToe[1][1]=='X'&&ttt265.game.TicTacToe[1][2]=='X')
    {
        ttt265.game.winFlag=TRUE;
    }
    if(ttt265.game.TicTacToe[2][0]=='X'&&ttt265.game.TicTacToe[2][1]=='X'&&ttt265.game.TicTacToe[2][2]=='X')
    {
        ttt265.game.winFlag=TRUE;
    }
    if(ttt265.game.TicTacToe[0][0]=='X'&&ttt265.game.TicTacToe[1][0]=='X'&&ttt265.game.TicTacToe[2][0]=='X')
    {
        ttt265.game.winFlag=TRUE;
    }
    if(ttt265.game.TicTacToe[0][1]=='X'&&ttt265.game.TicTacToe[1][1]=='X'&&ttt265.game.TicTacToe[2][1]=='X')
    {
        ttt265.game.winFlag=TRUE;
    }
    if(ttt265.game.TicTacToe[0][2]=='X'&&ttt265.game.TicTacToe[1][2]=='X'&&ttt265.game.TicTacToe[2][2]=='X')
    {
        ttt265.game.winFlag=TRUE;
    }
    if(ttt265.game.TicTacToe[0][0]=='X'&&ttt265.game.TicTacToe[1][1]=='X'&&ttt265.game.TicTacToe[2][2]=='X')
    {
        ttt265.game.winFlag=TRUE;
    }
    if(ttt265.game.TicTacToe[0][2]=='X'&&ttt265.game.TicTacToe[1][1]=='X'&&ttt265.game.TicTacToe[2][0]=='X')
    {
        ttt265.game.winFlag=TRUE;
    }

    if(ttt265.game.TicTacToe[0][0]=='O'&&ttt265.game.TicTacToe[0][1]=='O'&&ttt265.game.TicTacToe[0][2]=='O')
    {
        ttt265.game.winFlag=TRUE;
    }
    if(ttt265.game.TicTacToe[1][0]=='O'&&ttt265.game.TicTacToe[1][1]=='O'&&ttt265.game.TicTacToe[1][2]=='O')
    {
        ttt265.game.winFlag=TRUE;
    }
    if(ttt265.game.TicTacToe[2][0]=='O'&&ttt265.game.TicTacToe[2][1]=='O'&&ttt265.game.TicTacToe[2][2]=='O')
    {
        ttt265.game.winFlag=TRUE;
    }
    if(ttt265.game.TicTacToe[0][0]=='O'&&ttt265.game.TicTacToe[1][0]=='O'&&ttt265.game.TicTacToe[2][0]=='O')
    {
        ttt265.game.winFlag=TRUE;
    }
    if(ttt265.game.TicTacToe[0][1]=='O'&&ttt265.game.TicTacToe[1][1]=='O'&&ttt265.game.TicTacToe[2][1]=='O')
    {
        ttt265.game.winFlag=TRUE;
    }
    if(ttt265.game.TicTacToe[0][2]=='O'&&ttt265.game.TicTacToe[1][2]=='O'&&ttt265.game.TicTacToe[2][2]=='O')
    {
        ttt265.game.winFlag=TRUE;
    }
    if(ttt265.game.TicTacToe[0][0]=='O'&&ttt265.game.TicTacToe[1][1]=='O'&&ttt265.game.TicTacToe[2][2]=='O')
    {
        ttt265.game.winFlag=TRUE;
    }
    if(ttt265.game.TicTacToe[0][2]=='O'&&ttt265.game.TicTacToe[1][1]=='O'&&ttt265.game.TicTacToe[2][0]=='O')
    {
        ttt265.game.winFlag=TRUE;
    }
}
/*>>> updateDisplay >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
Author:     pl
Date:       04/10/2020
Modified:   Name or None
Desc:       this function is contain escap sequences to print the value to terminal and make the disition about which place is to take count
Input:      none
Returns:    none
>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/
void updateDisplay(void)
{
    if(ttt265.game.winFlag==FALSE)
    {
        if(ttt265.game.turnCounter==FALSE)
        {
            pc.printf("\e[1;0H\r TICTACTOE265 \t\t GAME TIME: %2d:%2d \t\tSTATUS: READY!!",ttt265.time[MIN],ttt265.time[SEC]);
        }
        else if(ttt265.game.turnCounter!=FALSE && ttt265.game.turnCounter<=9)
        {
            pc.printf("\e[1;0H\r TICTACTOE265 \t\t GAME TIME: %2d:%2d \t\tSTATUS: PLAYING!!",ttt265.time[MIN],ttt265.time[SEC]);
        }
    }
    if(ttt265.game.winFlag==TRUE)
    {
        pc.printf("\e[1;0H\r TICTACTOE265 \t\t GAME TIME: %2d:%2d \t\tSTATUS: WON!!",ttt265.time[MIN],ttt265.time[SEC]);
        ttt265.game.winFlag==FALSE;
    }
    if(PB4==FALSE)  
    {
        pc.printf("\e[1;0H\r TICTACTOE265 \t\t GAME TIME: %2d:%2d \t\tSTATUS: RESET!!",ttt265.time[MIN],ttt265.time[SEC]);
    }   
    pc.printf("\e[2;0H\r PLAYER :%c \t\t TURN COUNT: %d ",ttt265.game.player,ttt265.game.turnCounter);
    if(type==FALSE)
    {
        pc.printf("\e[3;0H\r ROW\t\t");   
    }
    if(type==1)
    {
        pc.printf("\e[3;0H\r COLUMN\t\t");
    }
    pc.printf("\e[4;0H\r SELECT: %d",ttt265.select);
    pc.printf("\e[5;0H\r \tC O L U M N");
    pc.printf("\e[6;0H\r \t   0  1  2");
    pc.printf("\e[7;0H\r \t0 %c | %c | %c |",ttt265.game.TicTacToe[0][0],ttt265.game.TicTacToe[0][1],ttt265.game.TicTacToe[0][2]);
    pc.printf("\e[8;0H\rR  \t--------------");
    pc.printf("\e[9;0H\rO  \t1 %c | %c | %c |",ttt265.game.TicTacToe[1][0],ttt265.game.TicTacToe[1][1],ttt265.game.TicTacToe[1][2]);
    pc.printf("\e[10;0H\rW  \t-----------------");
    pc.printf("\e[11;0H\r  \t2 %c | %c | %c |",ttt265.game.TicTacToe[2][0],ttt265.game.TicTacToe[2][1],ttt265.game.TicTacToe[2][2]);
    while(ttt265.game.winFlag==TRUE)
    {
        if(ttt265.game.turnCounter==0 ||ttt265.game.turnCounter==2 ||ttt265.game.turnCounter==4||ttt265.game.turnCounter==6||ttt265.game.turnCounter==8)
        {
            pc.printf("\e[13;0H\r PLAYER O IS WINNER");
            ttt265.game.winFlag=FALSE;
        }
        else if(ttt265.game.turnCounter==1 ||ttt265.game.turnCounter==3 ||ttt265.game.turnCounter==5||ttt265.game.turnCounter==7||ttt265.game.turnCounter==9)
        {
            pc.printf("\e[13;0H\r PLAYER X IS WINNER");
            ttt265.game.winFlag=FALSE;
        }
    }
}// eo updateDisplay::**************************************************************************************************
/*>>> fillPlace >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
Author:     pl
Date:       04/11/2020
Modified:   Name or None
Desc:       this function will track record of the game time
Input:      giving the value for the row and column position to check and fill the place
Returns:    none    
>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/
void fillPlace(char row1,char column1)
{
    row=row1;
    column=column1;
    if(ttt265.game.turnCounter==0 ||ttt265.game.turnCounter==2 ||ttt265.game.turnCounter==4||ttt265.game.turnCounter==6||ttt265.game.turnCounter==8)
    {
        ttt265.game.TicTacToe[row][column]='X';
        ttt265.turnTime=30;
        type=FALSE;
        ttt265.game.turnCounter++;                  
    }
    else
    {
        ttt265.game.TicTacToe[row][column]='O';
        ttt265.turnTime=30;
        type=FALSE;
        ttt265.game.turnCounter++;
    }
    row1=row;
    column1=column;   
}
//eo:fillSpace
/*>>> collectString >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
Author:     pl
Date:       04/10/2020
Modified:   Name or None
Desc:       this function is collecting sentences
Input:      none
Returns:    none
>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/
void collectString()
{
        char hold=FALSE;
        hold=pic265.getc();
        if(hold =='$')   
        {
            insert=FALSE;
        }
        if(hold =='^')
        {
            sentenceReady = TRUE;
            rxbuf1[insert+1]=0x00;
        }
        rxbuf1[insert]=hold;
        insert++;
}
//eo:: collectString===============================================================================================================
/*>>> validateString >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
Author:     pl
Date:       04/10/2020
Modified:   Name or None
Desc:       this function is validated the receiving string
Input:      referance pointer array
Returns:    none
>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/
char validateString(char *ptr)
{
    char rcs=0;
    char ncs=0;
    char csFlag=FALSE;
    int count=strlen(ptr);
    while(csFlag!=TRUE)
    {
        if(*(ptr+count)=='$')
        {
            *(ptr+count)='\0';
        }
        if(*(ptr+count)==',')
        {
            *(ptr+count)='\0';
            rcs=atoi(ptr+count+1);
            csFlag=TRUE;
        }
        count--;
    }//eo while
    
    ncs=checkSum(ptr);
    if(ncs==rcs)
    {
        return FALSE;
    }
    else 
    {
        return TRUE;
    }  

}

//eo:: validateString=============================================================================================================
/*>>> parsingString >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
Author:     pl
Date:       04/10/2020
Modified:   Name or None
Desc:       this function parse string into small part
Input:      referance pointer array
Returns:    none
>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/
void parsingString(char *ptr)
{
    int counter=0;
    while(*ptr)
    {
        if(*ptr=='$'||*ptr==',')
        {
            *ptr='\0';
            tokens[counter]=ptr+1;
            counter++;
        }
        ptr++;
    }
}
//eo:: parsingString===========================================================================
/*>>> executeString >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
Author:     pl
Date:       04/10/2020
Modified:   Name or None
Desc:       this functionexecute the parsed string and make changes
Input:      none
Returns:    none
>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/
void executeString()
{
    char row2;
    char column2;
    char time=0;
    if(atoi(tokens[2])==265)
    {
        if(strcmp(tokens[0],"PLYPMT")==0)
        {
                row2=atoi(tokens[3]);
                column2=atoi(tokens[4]);
                fillPlace(row2,column2);
        }
        if(strcmp(tokens[0],"GAMEND")==0)
        {
            time=atoi(tokens[5]);
            ttt265.time[MIN]=time;     
        }
    }
}

/*>>> baudRate >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
Author:     pl
Date:       04/10/2020
Modified:   Name or None
Desc:       this function is contain escap sequences to print the value to terminal and make the disition about which place is to take count
Input:      none
Returns:    none
>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/
void baudRate()
{
    pc.baud(9600);
    pic265.baud(19200); 
}
/*>>> timer3 >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
Author:     pl
Date:       04/10/2020
Modified:   Name or None
Desc:       this function is count 3 second in mbed
Input:      none
Returns:    none
>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/
void timer3()
{
    if(timerResetFlag)
    {
        timerReset++;
    }
}

int main() 
{
    initialTtt();
    t.attach(&timer3,1.0);
    baudRate();
    pic265.attach(&collectString);
    while(ttt265.game.turnCounter<=9)
    {
        winCheck();
        userInput();
        updateDisplay();
        playerReturn();
        pc.printf("\e[16;0H\r %s",rxbuf1);
        if(sentenceReady==TRUE)
        {
             sentenceReady=FALSE; 
             validateString(rxbuf1);
             parsingString(rxbuf1);
             executeString();
        }  
        
    }
}



PIC CODE=========================================================
=================================================================
=================================================================


	//>>> ELNC6007 2020F LAB4  >>>
/*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
	File Name:	ELNC6007265Lab4.c
	Author:		PARTH LAD
	Date:		25/11/2020
	Modified:	None
	c Fanshawe College, 2020

	Description: *

* THIS CODE IS ABOUT THE SETUP FOR THE MICROCONTROLLER.
* THUS IS GAME OF TIC TAC TOE WHERE TWO PLAYER CAN PLAY THE GAME AND DECLARE THE WINNER BY USER INPUT
* ALSO CONTROLL THE MULTIPLE LEDS ACCORDING TO USER INPUT FROM THE PUSH BUTTON
* THIS PROGRAM ALSO DISPLAY THE TIME 
* AND ALSO RESET THE GAME IF MODE AND ENTER IS PRESSED FOR 3 SECONDS
* ALSO DIPLAY ONE STRING FOR 5 SECONDS AND COUNT DOWN DISPLAY.
	
>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/

// Preprocessor >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

// Libraries >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
#include "pragmas.h"
#include <stdlib.h>
#include <stdio.h>
#include <p18f45k22.h>
#include <string.h>

// Constants >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
#define TRUE	     	1
#define	FALSE		    0	
#define PBMASK          0xF0
#define PBS             PORTA & PBMASK
#define NOPRESS         0xF0
#define SEC             0
#define MIN             1
#define T0FLAG          INTCONbits.TMR0IF   //to make the go flag up
#define T2FLAG          INTCONbits.TMR2IF
#define POSITION00      PORTDbits.RD0       // define the port bits for the LED 00
#define POSITION01      PORTDbits.RD3       // define the port bits for the LED 01
#define POSITION02      PORTCbits.RC4       // define the port bits for the LED 02
#define POSITION10      PORTDbits.RD1		// define the port bits for the LED 10
#define POSITION11      PORTDbits.RD4		// define the port bits for the LED 11
#define POSITION12      PORTCbits.RC3		// define the port bits for the LED 12
#define POSITION20      PORTDbits.RD2		// define the port bits for the LED 20
#define POSITION21      PORTDbits.RD5		// define the port bits for the LED 21
#define POSITION22      PORTCbits.RC5		// define the port bits for the LED 22
#define OPEN00          TRISD&=0XFE			// define the port bits for adjust tris value
#define OPEN01          TRISD&=0XF7			// define the port bits for adjust tris value
#define OPEN02          TRISC&=0XEF			// define the port bits for adjust tris value
#define OPEN10          TRISD&=0XFD			// define the port bits for adjust tris value
#define OPEN11          TRISD&=0XEF			// define the port bits for adjust tris value
#define OPEN12          TRISC&=0XF7			// define the port bits for adjust tris value
#define OPEN20          TRISD&=0XFB			// define the port bits for adjust tris value
#define OPEN21          TRISD&=0XDF			// define the port bits for adjust tris value
#define OPEN22          TRISC&=0XDF			// define the port bits for adjust tris value
#define ADRES1          265
#define TTTRECEIVE      1 
#define TTTSEND         265
#define BUFSIZE         25
#define RC2FLAG         PIR3bits.RC2IF

// Global Variables >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
int row=0;				//variable to hold the value of row
int column=0;			// variable to hold the value of column
char type=0;            // variable to secelct row=0 or column=1
char index1=FALSE;		// variable for initialization loop
char index2=FALSE;		// variable for initialization loop
char secondHold=FALSE;
char SecondCount=FALSE;
char latState= FALSE;
char pbState=FALSE;
char PBRead=FALSE;
char count=FALSE;
char index3=0;
char index4=0;
char buf1[BUFSIZE];
char bufEnd[BUFSIZE];
char stringCount=0;
char stringFlag=FALSE;
char autoFlag=FALSE;
char sentenceReady=FALSE;
char insert=FALSE;
char rxBuf[BUFSIZE];
char*ptr;
char *token[BUFSIZE];
char cs=FALSE;
char rcs=FALSE;
char ncs=FALSE;
char csFlag=FALSE;
char plypmt[]={"PLYPMT"};
char gamrst[]={"GAMRST"};
typedef struct gameBoard
{
	char TicTacToe[3][3];                //declare 2d array for the game board
	char player;                        // declare the variable to indicate which player is active
	int turnCounter;                     // variable to count the how many turns have been taken 
	int winFlag;                        // true flase variable tht indicate the a win cordinate       
}game265_t;

typedef struct TicTacToeSystem
{
	int address;                          //variable to hold the student id number 
	game265_t game;
	int select;                           // variable that select between row and column
	char time[2];                           // variable array that holds minuits and second
	char status;                           //indicator varible which display current ststus of game
	char turnTime;
}ttt_t;

ttt_t ttt265;

/*>>> interrupt >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
Author:		pl
Date:		04/12/2020
>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/
void ISR(void);
#pragma code int_vector=0x0008
void int_vector()
{
	_asm
		GOTO ISR
	_endasm
}
#pragma code

// Functions >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
/*>>> intConfig >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
Author:		pl
Date:		04/12/2020
Modified:	Name or None
Desc:		this function will set the interrupt configuration for pic
Input: 		none
Returns:	none
>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/
void intConfig()
{
	INTCON|=0x20;    //set the INTCON bit 5 for the configuration
	INTCON2&=0x04;   // setINTCON2 bit 2 for the configuration
	PIR3bits.RC2IF=FALSE; // set PIR register bit 3 low for receiving falg low
	IPR3 |= 0x20;         //Set IPR2 bit 5 
	PIE3 |=0x20;		// set PIE3 bit 5
	INTCON|=0xC0;       //reset all interrupt 
}
//Eo: intConfig==============================================================================
/*>>> setOSCfreq >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
Author:		pl
Date:		01/10/2020
Modified:	Name or None
Desc:		This function is help to set the frequency for the oscillator for 4mhz 
Input: NONE
Returns:	NONE
>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/
void setOSCfreq(void)
{
	OSCCON  =  	0x52; //Sleep on slp cmd, HFINT 4MHz, INT OSC Blk TABLE NUMBER 2-4 PAGE NUMBER 43
	OSCCON2 = 	0x04;	// PLL No, Sec OSC off, Pri OSC
	OSCTUNE = 	0x80;	// PLL disabled, Default factory 
	
	while (OSCCONbits.HFIOFS != 1); 	// wait for osc to become stable

}// eo setOSCfreq::**************************************************************************************************
/*>>> portConfig >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
Author:		pl
Date:		02/10/2020
Modified:	Name or None
Desc:		configure the ports of microcontroller
Input: 		NONE
Returns:	NONE
>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/
void portConfig(void)
{
		//PORT A

		ANSELA = 0x00;     // SET PORT A AS ANALOG
		LATA   = 0x00;     // SET THE PORT A AS OUTPUT
		TRISA  = 0xFF;     // SET AS INPUT

	//PORT B

		ANSELB = 0x00;     //SET PORT B AS DIGITAL
		LATB   = 0x00;      
		TRISB  = 0x00;     // SET PORT B AS OUTPUT
	//PORT C

		ANSELC = 0x00;     //SET PORT C AS DIGITAL
		LATC   = 0x00;      
		TRISC  = 0xFF;     // SET PORT C AS input for led off position
	//PORT D

		ANSELD = 0x00;     // SET PORT D AS DIGITAL
		LATD   = 0x00;     // SET PORT D OUTPUT
		TRISD  |= 0xFF;    // SET PORT D AS input for led off position and for serial 2 configuration

}// eo portConfig::*****************************************************************************************************

/*>>> serialConfig >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
Author:		pl
Date:		02/10/2020
Modified:	Name or None
Desc:		* Set the configuration for the srial communication with rx232 ***
Input: 		none
Returns:	none
>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/
void serialConfig()
{
		SPBRG1= 25;                // set the baud rate value 9600
	 	TXSTA1= 0X26;              // set the tx
		RCSTA1= 0X90;              // set the rx
		BAUDCON1=0X40;             // select the baud config


}// eo serialConfig::********************************************************************************************************
/*>>> serialConfig2 >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
Author:		pl
Date:		02/10/2020
Modified:	Name or None
Desc:		* Set the configuration for the srial communication set 2 with mbed lpc1768 ***
Input: 		none
Returns:	none
>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/
void serialConfig2()
{
		SPBRG2= 12;                // set the baud rate value 19200
	 	TXSTA2= 0X26;              // set the tx
		RCSTA2= 0X90;              // set the rx
		BAUDCON2=0X40;             // select the baud config


}// eo serialConfig::********************************************************************************************************
/*>>> TMR0Reset >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
Author:		pl
Date:		02/10/2020
Modified:	Name or None
Desc:		reset timer 0 
Input: 		*** List arguments for this function or none ***
Returns:	*** List returning argument for this function or none ***
>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/
void TMR0Reset(void)
{
	T0FLAG=FALSE;       //making the timer0 flag 0
	TMR0H=0x0b;         // set the timer0 high bits
	TMR0L=0xdc;         // set the timer0 low bits

}// eo TMR0Reset::**************************************************************************************

/*>>> TMR0Config >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
Author:		pl	
Date:		02/10/2020
Modified:	Name or None
Desc:		set the timer 0 for 1 second count
Input: 		none
Returns:	none
>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/
void TMR0Config(void)
{
	T0CON=0x93;           // set the timer for the one second
	TMR0Reset();

}// eo TMR0Config::*****************************************************************************************

/*>>> initialTtt >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
Author:		pl
Date:		02/10/2020
Modified:	Name or None
Desc:	    it will initialize the variable of the global data structure
Input: 		none
Returns:	none
>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/
void initialTtt(void)
{
	//single variable
 
	ttt265.address=ADRES;
	ttt265.game.player=FALSE;
	ttt265.game.turnCounter=FALSE;
	ttt265.game.winFlag=FALSE;
	ttt265.select=FALSE;
	ttt265.turnTime=30;
	//multiple variable /array

	//use loop to initialize whole array and set the value at 0xff
	for(index1=0;index1<3;index1++)
	{
		for(index2=0;index2<3;index2++)
		{
			ttt265.game.TicTacToe[index1][index2]=0XFF;
		}
	}
	ttt265.time[SEC]=FALSE;
	ttt265.time[MIN]=FALSE;
}// eo initialTtt::*********************************************************************************************************
/*>>> playerReturn >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
Author:		Name
Date:		05/10/2020
Modified:	Name or None
Desc:		this function will determine according to player name either X or O from turn count
Input: 		none
Returns:	none
>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/
char playerReturn()
{
	if(ttt265.game.turnCounter==0 ||ttt265.game.turnCounter==2 ||ttt265.game.turnCounter==4||ttt265.game.turnCounter==6||ttt265.game.turnCounter==8)
	{
		ttt265.game.player='O';
		return 'O';
	}
	else
	{
		ttt265.game.player='X';
		return 'X';
	}
}// eo  playerReturn::
/*>>> checkSum >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
Author:     pl
Date:       04/12/2020
Modified:   Name or None
Desc:       this function will return the value of checksum in which i have use XOR logic to check the checksum for string
Input:      input always a pointer to start with
Returns:    it will return char value of checksum 
>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/
char checkSum(char *ptr)
{
    char cs=0;
    while(*ptr)
    {
        cs^=*ptr;
        ptr++;
    }
    return cs;
}
//EO: CHECKSUM
/*>>> LEDControl >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
Author:		pl
Date:		05/10/2020
Modified:	Name or None
Desc:		this function control leds according to the value pushed by user at mbed side and also with autoplacement
Input: 		none
Returns:	none
>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/
void LEDControl(void)
{
	if(ttt265.game.turnCounter==0 ||ttt265.game.turnCounter==2 ||ttt265.game.turnCounter==4||ttt265.game.turnCounter==6||ttt265.game.turnCounter==8) // even number for player X
	{
		//check the condition according to avlue od row and column
		if(row==0 && column==0)
		{
			OPEN00;
			POSITION00=TRUE;
		}
		else if(row==0 && column==1)
		{
			OPEN01;
			POSITION01=TRUE;
		}
		else if(row==0 && column==2)
		{
			OPEN02;
			POSITION02=TRUE;
		}
		else if(row==1 && column==0)
		{
			OPEN10;
			POSITION10=TRUE;
		}
		else if(row==1 && column==1)
		{
			OPEN11;
			POSITION11=TRUE;
		}
		else if(row==1 && column==2)
		{
			OPEN12;
			POSITION12=TRUE;
		}
		else if(row==2 && column==0)
		{
			OPEN20;
			POSITION20=TRUE;
		}
		else if(row==2 && column==1)
		{
			OPEN21;
			POSITION21=TRUE;
		}
		else if(row==2 && column==2)
		{
			OPEN22;
			POSITION22=TRUE;
		}
	}
	else
	{
		if(row==0 && column==0)
		{
			OPEN00;
			POSITION00=FALSE;
		}
		else if(row==0 && column==1)
		{
			OPEN01;
			POSITION01=FALSE;
		}
		else if(row==0 && column==2)
		{
			OPEN02;
			POSITION02=FALSE;
		}
		else if(row==1 && column==0)
		{
			OPEN10;
			POSITION10=FALSE;
		}
		else if(row==1 && column==1)
		{
			OPEN11;
			POSITION11=FALSE;
		}
		else if(row==1 && column==2)
		{
			OPEN12;
			POSITION12=FALSE;
		}
		else if(row==2 && column==0)
		{	
			OPEN20;
			POSITION20=FALSE;
		}
		else if(row==2 && column==1)
		{
			OPEN21;
			POSITION21=FALSE;
		}
		else if(row==2 && column==2)
		{
			OPEN22;
			POSITION22=FALSE;
		}	
	}
}// eo LEDControl::**************************************************************************************************************************************************

/*>>> initialSystem >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
Author:		pl
Date:		02/10/2020
Modified:	Name or None
Desc:		Initalizes the ttt System.
Input: 		none
Returns:	none
>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/
void initialSystem(void)
{
	setOSCfreq();
	serialConfig();
	portConfig();
	TMR0Config();
	TMR0Reset();
	initialTtt();
	serialConfig2();
	intConfig();
	row=0;
	column=0;
	type=0;
}// eo initialSystem::******************************************************************************************

/*>>> stringPrint >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
Author:		pl
Date:		04/12/2020
Modified:	Name or None
Desc:		this function will create a communication sentence while there is any auto placement is done 
Input: 		none
Returns:	none
>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/
void stringPrint()
{
    	sprintf(buf1,"$PLYPMT,%i,%i,%i,%i,%c",TTTRECEIVE,TTTSEND,row,column,playerReturn());
		sprintf(buf1,"%s,%i",buf1,checkSum(buf1));
		sprintf(buf1,"%s^",buf1);
		puts2USART(buf1);
		printf("\e[15;0H\ %s(<-NEW COMMUNICATION SENTANCE)",buf1); 
}

//eo:stringPrint*****************************************************************************************

/*>>> sendStringEnd>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
Author:		pl
Date:		04/12/2020
Modified:	Name or None
Desc:		this function will create a communication sentence while there is any decision or 
			end the game
Input: 		it will require input argument for the situation of game win/draw and other input 
			is playerwin in which it player O or X or none(N) 
Returns:	none
>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/
void sendStringEnd( char situation,char playerWin)
{
    sprintf(bufEnd,"$GAMEND,%i,%i,%c,%c,%2d%2d",TTTRECEIVE,TTTSEND,situation,playerWin,ttt265.time[MIN],ttt265.time[SEC]);
	sprintf(bufEnd,"%s,%i",bufEnd,checkSum(bufEnd));
	sprintf(bufEnd,"%s^",bufEnd);
	puts2USART(bufEnd);
	printf("\e[17;0H\ %s(<-NEW COMMUNICATION SENTANCE)",bufEnd); 
}

//eo:stringEnd********************************************************************************************
/*>>> winCheck >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
Author:		pl
Date:		03/10/2020
Modified:	Name or None
Desc:		it will check the all winning condition for the player1 and player2  
Input: 		it will take input as all the game board array menmber and check the winning condition after the turn count goes 5
Returns:	if the any of the winning condition is true the function will return the value according to winning condition
>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/
void winCheck()
{

		if(ttt265.game.TicTacToe[0][0]=='X'&&ttt265.game.TicTacToe[0][1]=='X'&&ttt265.game.TicTacToe[0][2]=='X')
		{
			ttt265.game.winFlag=TRUE;
		}
		if(ttt265.game.TicTacToe[1][0]=='X'&&ttt265.game.TicTacToe[1][1]=='X'&&ttt265.game.TicTacToe[1][2]=='X')
		{
			ttt265.game.winFlag=TRUE;
		}
		if(ttt265.game.TicTacToe[2][0]=='X'&&ttt265.game.TicTacToe[2][1]=='X'&&ttt265.game.TicTacToe[2][2]=='X')
		{
			ttt265.game.winFlag=TRUE;
		}
		if(ttt265.game.TicTacToe[0][0]=='X'&&ttt265.game.TicTacToe[1][0]=='X'&&ttt265.game.TicTacToe[2][0]=='X')
		{
			ttt265.game.winFlag=TRUE;
		}
		if(ttt265.game.TicTacToe[0][1]=='X'&&ttt265.game.TicTacToe[1][1]=='X'&&ttt265.game.TicTacToe[2][1]=='X')
		{
			ttt265.game.winFlag=TRUE;
		}
		if(ttt265.game.TicTacToe[0][2]=='X'&&ttt265.game.TicTacToe[1][2]=='X'&&ttt265.game.TicTacToe[2][2]=='X')
		{
			ttt265.game.winFlag=TRUE;
		}
		if(ttt265.game.TicTacToe[0][0]=='X'&&ttt265.game.TicTacToe[1][1]=='X'&&ttt265.game.TicTacToe[2][2]=='X')
		{
			ttt265.game.winFlag=TRUE;
		}
		if(ttt265.game.TicTacToe[0][2]=='X'&&ttt265.game.TicTacToe[1][1]=='X'&&ttt265.game.TicTacToe[2][0]=='X')
		{
			ttt265.game.winFlag=TRUE;
		}
		if(ttt265.game.TicTacToe[0][0]=='O'&&ttt265.game.TicTacToe[0][1]=='O'&&ttt265.game.TicTacToe[0][2]=='O')
		{
			ttt265.game.winFlag=TRUE;
		}
		if(ttt265.game.TicTacToe[1][0]=='O'&&ttt265.game.TicTacToe[1][1]=='O'&&ttt265.game.TicTacToe[1][2]=='O')
		{
			ttt265.game.winFlag=TRUE;
		}
		if(ttt265.game.TicTacToe[2][0]=='O'&&ttt265.game.TicTacToe[2][1]=='O'&&ttt265.game.TicTacToe[2][2]=='O')
		{
			ttt265.game.winFlag=TRUE;
		}
		if(ttt265.game.TicTacToe[0][0]=='O'&&ttt265.game.TicTacToe[1][0]=='O'&&ttt265.game.TicTacToe[2][0]=='O')
		{
			ttt265.game.winFlag=TRUE;
		}
		if(ttt265.game.TicTacToe[0][1]=='O'&&ttt265.game.TicTacToe[1][1]=='O'&&ttt265.game.TicTacToe[2][1]=='O')
		{
			ttt265.game.winFlag=TRUE;
		}
		if(ttt265.game.TicTacToe[0][2]=='O'&&ttt265.game.TicTacToe[1][2]=='O'&&ttt265.game.TicTacToe[2][2]=='O')
		{
			ttt265.game.winFlag=TRUE;
		}
		if(ttt265.game.TicTacToe[0][0]=='O'&&ttt265.game.TicTacToe[1][1]=='O'&&ttt265.game.TicTacToe[2][2]=='O')
		{
			ttt265.game.winFlag=TRUE;
		}
		if(ttt265.game.TicTacToe[0][2]=='O'&&ttt265.game.TicTacToe[1][1]=='O'&&ttt265.game.TicTacToe[2][0]=='O')
		{
			ttt265.game.winFlag=TRUE;
		}
}// eo winCheck::**********************************************************************************************************
/*>>> updateDisplay >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
Author:		pl
Date:		04/10/2020
Modified:	Name or None
Desc:		this function is contain escap sequences to print the value to terminal and make the disition about which place is to take count
Input: 		none
Returns:	none
>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/
void updateDisplay(void)
{
	if(ttt265.game.winFlag==FALSE)
	{
		if(ttt265.game.turnCounter==FALSE)
		{
			printf("\e[1;0H\r TICTACTOE265 \t\t GAME TIME: %2d:%2d \t\tSTATUS: READY!!",ttt265.time[MIN],ttt265.time[SEC]);
		}
		else if(ttt265.game.turnCounter!=FALSE && ttt265.game.turnCounter<=9)
		{
			printf("\e[1;0H\r TICTACTOE265 \t\t GAME TIME: %2d:%2d \t\tSTATUS: PLAYING!!",ttt265.time[MIN],ttt265.time[SEC]);
		}
	}
	if(ttt265.game.winFlag==TRUE)
	{
		printf("\e[1;0H\r TICTACTOE265 \t\t GAME TIME: %2d:%2d \t\tSTATUS: WON!!",ttt265.time[MIN],ttt265.time[SEC]);
	}	
	printf("\e[2;0H\r PLAYER :%c \t\t TURN COUNT: %d ",ttt265.game.player,ttt265.game.turnCounter);
	if(type==FALSE)
	{
		printf("\e[3;0H\r ROW\t\tTURN TIME:%2d",ttt265.turnTime);	
	}
	if(type==1)
	{
		printf("\e[3;0H\r COLUMN\t\tTURN TIME:%2d",ttt265.turnTime);
	}
	printf("\e[4;0H\r SELECT: %d",ttt265.select);
	printf("\e[5;0H\r \tC O L U M N");
	printf("\e[6;0H\r \t   0  1  2");
	printf("\e[7;0H\r \t0 %c | %c | %c |",ttt265.game.TicTacToe[0][0],ttt265.game.TicTacToe[0][1],ttt265.game.TicTacToe[0][2]);
	printf("\e[8;0H\rR  \t--------------");
	printf("\e[9;0H\rO  \t1 %c | %c | %c |",ttt265.game.TicTacToe[1][0],ttt265.game.TicTacToe[1][1],ttt265.game.TicTacToe[1][2]);
	printf("\e[10;0H\rW  \t-----------------");
	printf("\e[11;0H\r  \t2 %c | %c | %c |",ttt265.game.TicTacToe[2][0],ttt265.game.TicTacToe[2][1],ttt265.game.TicTacToe[2][2]);
	while(ttt265.game.winFlag==TRUE)
	{
		if(ttt265.game.turnCounter==0 ||ttt265.game.turnCounter==2 ||ttt265.game.turnCounter==4||ttt265.game.turnCounter==6||ttt265.game.turnCounter==8)
		{
			printf("\e[13;0H\r PLAYER O IS WINNER");
			sendStringEnd('W','O');
		
		}
		else if(ttt265.game.turnCounter==1 ||ttt265.game.turnCounter==3 ||ttt265.game.turnCounter==5||ttt265.game.turnCounter==7||ttt265.game.turnCounter==9)
		{
			printf("\e[13;0H\r PLAYER X IS WINNER");
			sendStringEnd('W','X');
		}
	}
}// eo updateDisplay::******************************************************************************************************************
/*>>> clock >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
Author:		pl
Date:		18/10/2020
Modified:	Name or None
Desc:		this function will track record of the game time which call when timer 
			interrupt is happen
Input: 		none
Returns:	none
>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/
void clock()
{
	ttt265.time[SEC]++;
	if(ttt265.time[SEC]>=60)
	{
		ttt265.time[SEC]=FALSE;
		ttt265.time[MIN]++;
	}
}
//eo: clock********************************************************************************************
/*>>> fillPlace >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
Author:		pl
Date:		04/11/2020
Modified:	Name or None
Desc:		this function will track record of the game time
Input: 		giving the value for the row and column position to check and fill the place
Returns:    none	
>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/
void fillPlace(char row1,char column1)
{
	row=row1;
	column=column1;
	if(ttt265.game.turnCounter==0 ||ttt265.game.turnCounter==2 ||ttt265.game.turnCounter==4||ttt265.game.turnCounter==6||ttt265.game.turnCounter==8)
	{
		ttt265.game.TicTacToe[row][column]='X';
		ttt265.turnTime=30;
		type=FALSE;
		LEDControl();   //function call foe led control
		stringFlag=TRUE;
		ttt265.game.turnCounter++;
		autoFlag=FALSE;						
	}
	else
	{
		ttt265.game.TicTacToe[row][column]='O';
		ttt265.turnTime=30;
		type=FALSE;
		LEDControl();   //function call foe led control
		stringFlag=TRUE;
		ttt265.game.turnCounter++;
		autoFlag=FALSE;
	}
	row1=row;
	column1=column;
}
//eo:fillSpace*****************************************************************************************************
/*>>> turnTime >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
Author:		pl
Date:		04/11/2020
Modified:	Name or None
Desc:		this function will track record of the time of player turn
Input: 		none
Returns:	none
>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/
void turnTime()
{
	ttt265.turnTime--;
	if(ttt265.turnTime<=0)
	{
		autoFlag=TRUE;
		while(autoFlag==TRUE)
		{
	
			if(ttt265.game.TicTacToe[0][0]!='X' && ttt265.game.TicTacToe[0][0]!='O')
			{
				fillPlace(0,0);	
			}
			else if(ttt265.game.TicTacToe[0][1]!='X' && ttt265.game.TicTacToe[0][1]!='O')
			{
				fillPlace(0,1);	
			}
			else if(ttt265.game.TicTacToe[0][2]!='X' && ttt265.game.TicTacToe[0][2]!='O')
			{
				fillPlace(0,2);	
			}
			else if(ttt265.game.TicTacToe[1][0]!='X' && ttt265.game.TicTacToe[1][0]!='O')
			{
				fillPlace(1,0);	
			}
			else if(ttt265.game.TicTacToe[1][1]!='X' && ttt265.game.TicTacToe[1][1]!='O')
			{
				fillPlace(1,1);	
			}
			else if(ttt265.game.TicTacToe[1][2]!='X' && ttt265.game.TicTacToe[1][2]!='O')
			{
				fillPlace(1,2);	
			}
			else if(ttt265.game.TicTacToe[2][0]!='X' && ttt265.game.TicTacToe[2][0]!='O')
			{
				fillPlace(2,0);	
			}
			else if(ttt265.game.TicTacToe[2][1]!='X' && ttt265.game.TicTacToe[2][1]!='O')
			{
				fillPlace(2,1);	
			}
			else if(ttt265.game.TicTacToe[2][2]!='X' && ttt265.game.TicTacToe[2][2]!='O')
			{
				fillPlace(2,2);	
			}
			stringPrint();
		}
	}
}
//eo:turnTime************************************************************************************************
/*>>> collectString >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
Author:		pl
Date:		04/11/2020
Modified:	Name or None
Desc:		this function will collect the string/communication String stops when find any 
			string end character"^".
Input: 		this function 
Returns:	none
>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/
void collectString(char *ptr)
{
	char hold;
	while(sentenceReady==0)
	{
		if(RC2FLAG)
		{
			hold=RCREG2;
			if(hold=='$')
			{
				insert=0;
			}
			if(hold=='^')
			{
				sentenceReady=TRUE;
				rxBuf[insert+1]=0x00;	
			}
			rxBuf[insert]=hold;
			insert++;	
		}
	}
}
/*>>> ISR >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
Author:		pl
Date:		04/11/2020
Modified:	Name or None
Desc:		this function will call when there is any interrupt is occure
Input: 		none
Returns:	none
>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/
#pragma interrupt ISR
void ISR()
{
	if(T0FLAG)
	{
		T0FLAG= FALSE;
		clock();
		turnTime();
	}
	if(RC2FLAG)
	{
		PIE3bits.RC2IE=FALSE;
		sentenceReady=FALSE;
		collectString(rxBuf);
		PIE3bits.RC2IE=TRUE;
		RC2FLAG=FALSE;
	}
	INTCON|=0xC0;
}
/*>>> validateString >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
Author:		pl
Date:		04/11/2020
Modified:	Name or None
Desc:		this function will validate the receive string 
Input: 		pointer araay
Returns:	none
>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/
char validateString(char *ptr)
{
    int count=strlen(ptr);
    while(csFlag!=TRUE)
    {
        if(*(ptr+count)=='$')
        {
            *(ptr+count)='\0';
        }
        if(*(ptr+count)==',')
        {
            *(ptr+count)='\0';
            rcs=atoi(ptr+count+1);
            csFlag=TRUE;
        }
        count--;
    }//eo while
    ncs=checkSum(ptr);
    if(ncs==rcs)
    {
        return TRUE;
    }
    else 
    {
        return TRUE;
    } 
}
/*>>> parsingString >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
Author:		pl
Date:		04/11/2020
Modified:	Name or None
Desc:		this function will parse the receive the string
Input: 		referance araay
Returns:	none
>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/
void parsingSring(char *ptr)
{
	char counter=0;
	while(*ptr)
	{
		if(*ptr=='$'||*ptr==',')
		{
			*ptr=0x00;
			token[counter]=ptr+1;
			counter++;
		}
		ptr++;
	}
}
/*>>> exeuteString >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
Author:		pl
Date:		04/11/2020
Modified:	Name or None
Desc:		this function will execute the parsed string 
Input: 		none
Returns:	none
>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/
void executeString()
{
	char row2;
	char column2;
	if(atoi(token[2])==1)
	{
		if(atoi(token[3])==265)
		{
			if(strcmp(token[1],plypmt)==0)
			{
				row2=atoi(token[4]);
				column2=atoi(token[5]);
				fillPlace(row2,column2);
			}
		}
	}
	if(strcmp(token[1],gamrst)==0)
	{
		initialSystem();
	}
}
/*>>> MAIN: FUNCTION >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/
void main( void )
{
	initialSystem();
	while(ttt265.game.turnCounter<=9)
	{	
		
		if(sentenceReady==1)
		{
			sentenceReady=FALSE;
			if(validateString(rxBuf))
			{
				parsingSring(rxBuf);
				executeString();
			}
		}	
		if(ttt265.game.turnCounter==9)
		{
			sendStringEnd('D','N');
		}
		winCheck();
		updateDisplay();
	}
// eo main::
}