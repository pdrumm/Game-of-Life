#include <stdio.h>
#include <string.h>
#define boardSIZE 40
void zeroBoard(char [][boardSIZE]);
int interactiveMode(char [][boardSIZE]);
void fileMode(char [][boardSIZE], char []);
void editCell(char [][boardSIZE], char);
void fileEditCell(char [][boardSIZE], char, FILE *);
void printBoard(char [][boardSIZE]);
int genNewBoard(char [][boardSIZE]);
void checkSurr(char [][boardSIZE], int [][boardSIZE]);
int notOutBounds(int, int);

int main(int argc, char *argv[])
{
	// declare vairables
	char board[boardSIZE][boardSIZE]={0};
	int 	userPlay=1,	// is 0 if user wants to quit
		isSame=1;	// is 0 if the updated board is the same as the previous board
	zeroBoard(board);	// fills entire initial board with blank spaces
	if(argc==1) userPlay=interactiveMode(board);	// interactive mode
	else if(argc==2){				// file mode
		if(!fopen(argv[1],"r")){
			printf("Error: File %s could not be opened.\n",argv[1]);
			return 0;
		}
		fileMode(board,argv[1]);
	}
	else if(argc > 2){
		printf("Error: Too many arguments entered in command line\n");
		return 0;
	}

	if(userPlay==0) return 0;	// user chooses to quit game
	printBoard(board);		// prints board before it begins to update
	while(1){
		isSame=genNewBoard(board);
		printBoard(board);	// prints updated board
		if(isSame==0){
			printf("\nLife has reached an equilibrium!\n");
			return 0;
		}
		usleep(150000);
	}
}

void zeroBoard(char board[boardSIZE][boardSIZE])
{
	int i,j;
	for (i=0;i<boardSIZE;i++){
		for (j=0;j<boardSIZE;j++){
			board[i][j]=' ';
		}
	}
}

int interactiveMode(char board[boardSIZE][boardSIZE])
{
	char userIn[10];	// inputted user choice
	printf("This is the Game of Life!\n");
	printf("Would you like to:\n  'a': Add a new live cell\n  'r': Remove a live cell\n  's': Show the current board without advancing\n  'n': Advance the simulation by one generation\n  'o': Display options again\n  'q': Quit the program\n  'p': Play the game continuously\n");
	while(userIn[0] != 'q' && userIn[0] != 'p'){
		printf("Your choice: ");
		scanf("%s",userIn);
		switch(tolower(userIn[0])){
			case 'a':	// add live cell
				editCell(board,userIn[0]);
				break;
			case 'r':	// remove live cell
				editCell(board,userIn[0]);
				break;
			case 's':	// show board
				printBoard(board);
				break;
			case 'n':	// generate and print next generation
				genNewBoard(board);
				printBoard(board);
				break;
			case 'o':	// display options
				printf("Would you like to:\n  'a': Add a new live cell\n  'r': Remove a live cell\n  's': Show the current board without advancing\n  'n': Advance the simulation by one generation\n  'o': Display options again\n  'q': Quit the program\n  'p': Play the game continuously\n");
				break;
			case 'q':	// quit program
				break;
			case 'p':	// play game of life continuously
				break;
			default:	// wrong character inputted
				printf("That is not a valid input. Please enter a character listed in the menu.\n");
				break;
		}
	}
	if(userIn[0]=='q') return(0);
	if(userIn[0]=='p') return(1);
}

void fileMode(char board[boardSIZE][boardSIZE], char argv[30])
{
	char userIn;	// file inputted option
	FILE *fin = fopen(argv,"r");
	printf("This is the Game of Life!\n");
	while(userIn != 'p'){
		fscanf(fin,"%c",&userIn);
		if(userIn=='\n') continue;	//ignores the '\n' from when user hits enter
		switch(tolower(userIn)){
			case 'a':
				fileEditCell(board,userIn,fin);
				break;
			case 'p':
				break;
			default:
				printf("That is not a valid input. Please enter a character listed in the menu.\n");
				break;
		}
	}
}

void editCell(char board[boardSIZE][boardSIZE], char userIn)
{
	int xNew, yNew;	// coordinates of the new cell added or removed
	printf("Please enter the coordinates:\n x: ");
	scanf("%i",&xNew);
	printf(" y: ");
	scanf("%i",&yNew);
	if(xNew>(boardSIZE-1) || xNew<0 || yNew>(boardSIZE-1) || yNew<0){
		printf("Coordinates are outside of board. Cell not placed.\n");
		return;
	}
	if(userIn=='a') board[yNew][xNew]='X';
	if(userIn=='r') board[yNew][xNew]=' ';
}

void fileEditCell(char board[boardSIZE][boardSIZE], char userIn, FILE *fin)
{
	int xNew, yNew;	// coordinates of the new cell added or removed
	fscanf(fin,"%i",&xNew);
	fscanf(fin,"%i",&yNew);
	if(xNew>(boardSIZE-1) || xNew<0 || yNew>(boardSIZE-1) || yNew<0){
		printf("Coordinates are outside of board. Cell not placed.\n");
		return;
	}
	if(userIn=='a') board[yNew][xNew]='X';
	if(userIn=='r') board[yNew][xNew]=' ';
}

void printBoard(char board[boardSIZE][boardSIZE])
{
	system("clear");
	int i,j;
	printf("   ");
	for (i=0;i<boardSIZE;i++)
		printf("_");
	printf("\n");
	for (i=0;i<boardSIZE;i++){
		printf("%2i|",i);
		for (j=0;j<boardSIZE;j++){
			printf("%c",board[i][j]);
		}
		printf("\n");
	}
}
// RULES
// 1.Any live cell with less than two live neighbours dies
// 2.Any live cell with two or three live neighbours lives on to the next generation.
// 3.Any live cell with more than three live neighbours dies
// 4.Any dead cell with exactly three live neighbours becomes a live cell
int genNewBoard(char board[boardSIZE][boardSIZE])
{
	// 'X' means cell is alive. ' ' means cell is dead
	char tempBoard[boardSIZE][boardSIZE];
	int i, j, neighCount[boardSIZE][boardSIZE], isSame=0;
	checkSurr(board,neighCount);	// places the number of neighbors each cell has in its corresponding location in neighCount

	//Checks how rules of game apply to each cell
	for(i=0;i<boardSIZE;i++){
		for(j=0;j<boardSIZE;j++){
			if(board[j][i]=='X'){
				if(neighCount[j][i]<2 || neighCount[j][i]>3)
					tempBoard[j][i]=' ';
				else
					tempBoard[j][i]='X';
			}else{
				if(neighCount[j][i]==3)
					tempBoard[j][i]='X';
				else
					tempBoard[j][i]=' ';
			}
		}
	}

	//Now that temporary board has been created based off previous board, replace previous board with the temporary one
	for(i=0;i<boardSIZE;i++)
		for(j=0;j<boardSIZE;j++){
			if(board[j][i]!=tempBoard[j][i]) isSame++;
			board[j][i]=tempBoard[j][i];
		}
	return(isSame);
}

void checkSurr(char board[boardSIZE][boardSIZE], int neighCount[boardSIZE][boardSIZE])
{
	int i,j,currentNeigh,xBoard,yBoard;
	
	for(i=0;i<boardSIZE;i++){
		for(j=0;j<boardSIZE;j++){
			xBoard=j;
			yBoard=i;
			currentNeigh=0;
			//top left corner
			xBoard-=1;
			yBoard-=1;
			if(notOutBounds(xBoard,yBoard))
				currentNeigh = (board[xBoard][yBoard] == 'X')?(currentNeigh+1):currentNeigh;
			//above
			xBoard+=1;
			if(notOutBounds(xBoard,yBoard))
				currentNeigh = (board[xBoard][yBoard] == 'X')?(currentNeigh+1):currentNeigh;
			//top right corner
			xBoard+=1;
			if(notOutBounds(xBoard,yBoard))
				currentNeigh = (board[xBoard][yBoard] == 'X')?(currentNeigh+1):currentNeigh;
			//left
			xBoard-=2;
			yBoard+=1;
			if(notOutBounds(xBoard,yBoard))
				currentNeigh = (board[xBoard][yBoard] == 'X')?(currentNeigh+1):currentNeigh;
			//right
			xBoard+=2;
			if(notOutBounds(xBoard,yBoard))
				currentNeigh = (board[xBoard][yBoard] == 'X')?(currentNeigh+1):currentNeigh;
			//bottom left corner
			xBoard-=2;
			yBoard+=1;
			if(notOutBounds(xBoard,yBoard))
				currentNeigh = (board[xBoard][yBoard] == 'X')?(currentNeigh+1):currentNeigh;
			//below
			xBoard+=1;
			if(notOutBounds(xBoard,yBoard))
				currentNeigh = (board[xBoard][yBoard] == 'X')?(currentNeigh+1):currentNeigh;
			//bottom right corner
			xBoard+=1;
			if(notOutBounds(xBoard,yBoard))
				currentNeigh = (board[xBoard][yBoard] == 'X')?(currentNeigh+1):currentNeigh;
			//assign total number of neighbors to corresponding location in neighbor array
			neighCount[j][i]=currentNeigh;
		}
	}
}

int notOutBounds(int xBoard, int yBoard)
{
	// checks to make sure the neighbor location being checked for is not off of the board
	int notOB=1;
	if(xBoard<0 || xBoard>(boardSIZE-1)) notOB=0;
	if(yBoard<0 || yBoard>(boardSIZE-1)) notOB=0;
	return(notOB);
}
