//
// Created by Tomasz Mazurek
// [NZI-PK, gr. 1]

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
/*
 *  1 , 2, 3, 4
 *  5 , 6, 7, 8
 *  9 ,10,11,12
 *  13,14,15,16
 *
 *  dir{1, 4, 3, 5}
 *
 *  1 , 2, 3, 4, 5
 *  6 , 7, 8, 9,10
 *  11,12,13,14,15
 *  16,17,18,19,20
 *  21,22,23,24,25
 *
 * dir{1, 5, 4, 6}
 *
 *  1 , 2, 3, 4, 5, 6
 *  7 , 8, 9,10,11,12
 *  13,14,15,16,17,18
 *  19,20,21,22,23,24
 *  25,26,27,28,29,30
 *  31,32,33,34,35,36
 *
 *  dir{1, 6, 5, 7}
 *
 *  1 , 2, 3, 4, 5, 6, 7
 *  8 , 9,10,11,12,13,14
 *  15,16,17,18,19,20,21
 *  22,23,24,25,26,27,28
 *  29,30,31,32,33,34,35
 *  36,37,38,39,40,41,42
 *  43,44,45,46,47,48,49
 *
 *  dir{1, 7, 6, 8}
 *
 *
 * */

enum {NOUGHTS, CROSSES, BORDER, EMPTY};

void GameLoop(int size, int *board);
void InitializeBoard(int *board, int size, int *convertToGameSize);
void DisplayBoard(int *board, int size, int *convertToGameSize);
void ConvertToGameSize(int *board, int size, int *convertToGameSize);
void MakeMove(int *board, const int square, const int side);
int HasEmpty(int *board, int *convertToGameSize, int size);
int GetHumanMove(int *board, int *convertToGameSize, int size);
int GetComputerMove(int *board, const int side, int *convertToGameSize, int directions[4], int size);
int CheckDirection(int startSquare, const int dir, const int *board, const int side);
int FindWinningRow(int *board, const int currentSquare, const int side, int *directions, int size);
int FindWinRowAllBoard(int *board, const int side, int *convertToGameSize, int *directions, int size);
int CheckForWin(int *board, const int side, int *convertToGameSize, int directions[4], int size);
int MinMax(int *board, int side, int *convertToGameSize, int *directions, int size, int *depth, int *positions, int *maxDepth);
void DisplayHelp();
void SaveFile(int *board, int size);
void LoadFile(char *filename, int **board,int *size);
void clearScreen();

int main(int argc, char *argv[]){
    int index;
    int size = 3;
    int *board = NULL;
    char *filename = NULL;
    char choice[2];
    if(argc > 1) {
        for (index = 1; index < argc; ++index) {
            if (strncmp(argv[index], "-h", 2) == 0) {
                DisplayHelp();
            } else if ((strncmp(argv[index], "-s", 2) == 0) && argv[index + 1] != 0){
                size = atoi(argv[index + 1]);
                index++;
            } else if ((strncmp(argv[index], "-i", 2) == 0) && argv[index + 1] != 0) {
                filename = argv[index + 1];
                index++;
            } else {
                printf("\nWprowadzono nieprawidłowe parametry\n\n");
                DisplayHelp();
                return 1;
            }
        }
    }
    if(filename != NULL){
        LoadFile(filename, &board, &size);
    } else{
        board = malloc((size + 2) * (size + 2) * sizeof(int));
    }
    GameLoop(size, board);
    do {
        printf("\nCzy chcesz zagrac jeszcze raz? (t/n) : ");
        fgets(choice, 2, stdin);
        fflush(stdin);
        if (strcmp(choice, "n") == 0) {
            break;
        }
        clearScreen();
        board = malloc((size + 2) * (size + 2) * sizeof(int));
        GameLoop(size, board);
    }while(strcmp(choice, "t") == 0);
    return 0;
}

void GameLoop(int size, int *board) {
    int gameOver = 0;
    int side = NOUGHTS;
    int move = 0;
    int border = size + 2;
    int directions[4] = {1, border, border - 1, border + 1};
    int *convertToGameSize = malloc(sizeof(int) * (size * size));
    ConvertToGameSize(board,size, convertToGameSize);
    if(board[0] != 2){
        InitializeBoard(board, size, convertToGameSize);
    }
    while (!gameOver){
        if(side==NOUGHTS){//pobierz ruch gracza, wykonaj ruch, zmien strone
            move = GetHumanMove(board, convertToGameSize, size);
            MakeMove(board, move, side);
            side = side ^ 1;
        }else{//pobierz ruch komputera, wykonaj ruch, zmien strone
            clearScreen();
            move = GetComputerMove(board, side, convertToGameSize, directions, size);
            MakeMove(board, move, side);
            side = side ^ 1;
        }
        if(FindWinningRow(board, move, side ^ 1, directions, size) == size){ //jezeli na tablicy sa trzy znaki w rzedzie - gameOver = 1
            printf("\n\nKoniec gry! ");
            gameOver = 1;
            if(side == NOUGHTS){
                printf("Wygrywa: komputer");
            } else{
                printf("Wygrywa: czlowiek");
            }
        }
        if(HasEmpty(board, convertToGameSize, size) == 0){ //jezeli na tablicy niema pustych pol - Draw
            printf("Koniec gry\t Jest remis");
            gameOver = 1;
        }
    }
    DisplayBoard(board, size, convertToGameSize);
    free(board);
    free(convertToGameSize);
}

void InitializeBoard(int *board, int size, int *convertToGameSize) {
    int index = 0;


    for (index = 0; index < ((size + 2)*(size + 2)) ; index++) {
        board[index] = BORDER;
    }

    for (index = 0; index < size*size; ++index) {
        board[convertToGameSize[index]] = EMPTY;
    }

}

void DisplayBoard(int *board, int size, int *convertToGameSize) {
    int index = 0;
    char signs[] = "OX|-";
    printf("\n\n  Plansza: %dx%d\n\n", size, size);
    for (index = 0; index < size*size ; index++) {
        if(index != 0 && index % size == 0){
            printf("\n\n");
        }
        printf("%4c", signs[board[convertToGameSize[index]]]);
    }
    printf("\n");
}

void ConvertToGameSize(int *board, int size, int *convertToGameSize) {
    int index = 0;
    int border = size + 2;
    int row = 0;
    int count = 0;
    int squareNumber = 0;

    for (index = border; index < (border * border) - border ; index++) {
        if(index % border == 0){
            row = index + 1;
            squareNumber = 0;
        } else if (squareNumber < size && count != (size * size)) {
            convertToGameSize[count++] = row + (squareNumber++);
        }
    }
}

void MakeMove(int *board, const int square, const int side) {
    board[square] = side;
}

int HasEmpty(int *board, int *convertToGameSize, int size) {
    int index;
    for (index = 0; index < size * size; ++index) {
        if (board[convertToGameSize[index]] == EMPTY) {
            return 1;
        }
    }
    return 0;
}

int GetHumanMove(int *board, int *convertToGameSize, int size) {
    char userInput[4];
    int moveOK = 0;
    int move = -1;
    DisplayBoard(board, size, convertToGameSize);
    while(moveOK == 0){
        printf("\nProsze wprowadzic numer pola:  ");
        fgets(userInput, 3, stdin);
        fflush(stdin);
        if(strncmp(userInput,"s",1) == 0){
            SaveFile(board,size);
            continue;
        }
        if(sscanf(userInput, "%d", &move) != 1){
            move = -1;
            printf("Wprowadzono niewlasciwe znaki\n");
            continue;
        }
        if(move < 1 || move > size*size){
            move = -1;
            printf("Niewlasciwy zasieg\n");
            continue;
        }
        move--; //indeksowanie do zera;

        if(board[convertToGameSize[move]] != EMPTY){
            move = -1;
            printf("Pole zajete \n");
            continue;
        }
        moveOK = 1;
    }
    return convertToGameSize[move];
}

int GetComputerMove(int *board, const int side, int *convertToGameSize, int directions[4], int size) {
    int depth = 0;
    int positions = 0;
    int maxDepth = 0;
    printf("\nKomputer wykonuje ruch ");
    int best = MinMax(board, side, convertToGameSize, directions, size, &depth, &positions, &maxDepth);
    printf("\nZakonczono wyszukiwanie. \nPrzeszukanych pozycji: %d\nglebokosc przeszukiwania:%d ", positions, maxDepth);
    return best;

}

int CheckDirection(int startSquare, const int dir, const int *board, const int side) {
    int found = 0;
    while(board[startSquare] != BORDER){
        if(board[startSquare] != side){
            break;
        }
        found++;
        startSquare += dir;
    }
    return found;
}

int FindWinningRow(int *board, const int currentSquare, const int side, int *directions, int size){
    /* Funkcja bierze kwadrat (currentSquare)ktory zaznaczyliśmy, sprawdza stronę (side)a następnie iteruje w kierunkach
     * ustalonych przez tablice (directions) dodajac kazdy napotkany znak side do zmiennej winningRow ktora jest zwracana
     */
    int dirIndex = 0;
    int winningRow = 1;

    for (dirIndex = 0; dirIndex < 4; ++dirIndex) {
        int dir = directions[dirIndex];
        winningRow += CheckDirection(currentSquare + dir, dir, board, side);
        winningRow += CheckDirection(currentSquare + dir * - 1, dir * - 1, board, side);
        if(winningRow == size){
            break;
        }
        winningRow = 1;
    }
    return winningRow;
}

int FindWinRowAllBoard(int *board, const int side, int *convertToGameSize, int *directions, int size) {
    //Funkcja przeszukuje przekazana tablice sprawdzajac czy znajduja sie na niej znaki zgodne z side,
    //w przypadku znalezienia znaku zgodnego z size kwadrat ten zostaje sprawdzony czy jest czescia rzedu zawierajacego
    //ilosc tych znakow rowna dlugosci tablicy. Jesli tak, zapisuje wartosc 1 do zmiennej win ktora nastepnie jest
    //zwracana

    int win = 0;
    int index = 0;
    for (index = 0; index < size * size; ++index) {
        if(board[convertToGameSize[index]] == side){
            if(FindWinningRow(board, convertToGameSize[index], side, directions, size) == size){
                win = 1;
                //DisplayBoard(board, size, convertToGameSize);
                break;
            }
        }
    }
    return win;
}

int CheckForWin(int *board, const int side, int *convertToGameSize, int directions[4], int size) {
    if(FindWinRowAllBoard(board, side, convertToGameSize, directions, size) != 0){
        return 1;
    }
    if(FindWinRowAllBoard(board, side ^ 1, convertToGameSize, directions, size) != 0){
        return -1;
    }
    return 0;
}

int MinMax(int *board, int side, int *convertToGameSize, int *directions, int size, int *depth, int *positions, int *maxDepth) {
    int moveList[size * size];
    int moveCount = 0;
    int bestScore = -2;
    int score = -2;
    int bestMove = -1;
    int index = 0;
    if(*depth > *maxDepth) *maxDepth = *depth;
    (*positions)++;
    if(*positions % 100000 == 0 ){
        printf("*");
    }
    if(*depth > 0){//sprawdz czy wygrana
        score = CheckForWin(board, side, convertToGameSize, directions, size);
        if(score != 0 || *positions > 10000000){
            return score;
        }
    }
    for (index = 0; index < size*size; ++index) {    //jesli nie ma wygranej, wypelnij liste mozliwych ruchów
        if(board[convertToGameSize[index]] == EMPTY){
            moveList[moveCount++] = convertToGameSize[index];
        }
    }
    for (index = 0; index < moveCount; ++index) {//a nastepnie iteruj po niej w poszukiwaniu najlepszego ruchu
        int move = moveList[index];
        board[move] = side;
        (*depth)++;
        score = -MinMax(board, side ^ 1, convertToGameSize, directions, size, depth, positions, maxDepth);
        if(score > bestScore) {
            bestScore = score;
            bestMove = move;
        }
        board[move] = EMPTY;
        (*depth)--;
    }
    if(moveCount == 0){//ruchy koncowe zwracaja najlepszy wynik
        bestScore = FindWinRowAllBoard(board, side, convertToGameSize, directions, size);
    }
    if(*depth != 0){
        return bestScore;
    }else{
        return bestMove;
    }
}

void DisplayHelp() {
    printf("Gra Kolko-Krzyzyk jest gra o sumie zerowej. Warunkiem zwyciestwa w grze jest uzyskanie na"
    "planszy ciagu znakow o dlugosci zadanego boku\nuzywajac do tego indeksow odnoszacych sie do pol na planszy "
    "wg ponizszego schematu (analogiczna plansza 3x3) \n\n"
"                     1 2 3\n"
"                     4 5 6\n"
"                     7 8 9\n"
"\nGra dodatkowo umozliwia zapis jej aktualnego stanu w trakcie rozgrywki. Stan zapisujemy z uzyciem malej litery 's' "
"wprowadzanej\nzamiast wartosci pola podczas wykonywania ruchu. Nastepnie zostajemy poproszeni o podanie nazwy pliku ze stanem gry.\n"
"\nGra umozliwia uruchomienie z nastepujacymi parametrami:\n"
    "-h <-- ten parametr wyswietla widoczna na ekranie pomoc\n"
    "-i [nazwa pliku z zapisem stanu gry] jest to opcja umozliwiajaca przywrocenie stanu zapisanego wczesniej podczas rozgrywki\n"
    "-s [rozmiar boku planszy] Opcja ta umozliwia ustawienie rozmiaru kwadratowej planszy poprzez wprowadzenie dlugosci jednego z jej boku.\n\n");
}

void SaveFile(int *board, int size) {
    char filename[20];
    printf("Prosze wprowadzic nazwe pliku (max 20 znakow): \t");
    scanf("%19s", filename);
    FILE *fptr = fopen(filename, "wb");
    if(fptr == NULL) {
        fprintf(stderr, "Błąd przetwarzania pliku %s, numer bledu: %d\n", filename, errno);
        exit(1);
    }
    fwrite(&size, sizeof(int), 1, fptr);
    fwrite(board, sizeof(int), (size + 2) * (size + 2), fptr);
    fclose(fptr);
    printf("Plik %s zapisany", filename);
}

void LoadFile(char *filename, int **board,int *size){
    FILE *fptr = fopen(filename, "rb");
    if(fptr == NULL) {
        fprintf(stderr, "Błąd przetwarzania pliku %s, numer bledu: %d\n", filename, errno);
        exit(1);
    }
    fread(size, sizeof(int), 1, fptr);
    *board = malloc((*size + 2) * (*size + 2) * sizeof(int));
    fread(*board, sizeof(int), (*size + 2) * (*size + 2), fptr);
    fclose(fptr);
}

void clearScreen(){
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}
