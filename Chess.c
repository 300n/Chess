#include <math.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>  

#define width 1000
#define height 1000
SDL_Renderer* renderer;
SDL_Window* window;
Mix_Chunk *movingsound;
Mix_Chunk* initgamesound;
Mix_Chunk* checksound;
Mix_Chunk* takesound;
Mix_Chunk* rocksound; 
Mix_Chunk* endgamesound;
const int Pawnvalue = 1;
const int Bishopvalue = 3;
const int Knightvalue = 3;
const int Rockvalue = 5;
const int Queenvalue = 9;

void SDL_ExitWithError(const char* code) 
{
    printf("%s > \n", code);
    SDL_Quit();
    exit(EXIT_FAILURE);
}

int CountMat(int* tab, int colour)
{
    int Mat = 0;
    if (colour == 1) {
        colour += 6;
    }
    for (int i = 0; i<64 ; i++) {
        if (tab[i] == 1+colour) {
            Mat += Pawnvalue;
        } else if(tab[i] == 2+colour) {
            Mat += Rockvalue;
        } else if (tab[i] == 3+colour) {
            Mat += Bishopvalue;
        } else if (tab[i] == 4+colour) {
            Mat += Knightvalue;
        } else if (tab[i] == 5+colour) {
            Mat += Queenvalue;
        }
    }
    return Mat;
}

int Eval(int* tab)
{
    int WhiteEval = CountMat(tab,0);
    int BlackEval = CountMat(tab,1);
    return WhiteEval-BlackEval;
}

int* FEN(char* Fen,int *tab) 
{
    int k = 0;
    for (int i = 0; i<strlen(Fen) ; i++) {
        if (Fen[i] == 'R') {
            tab[k] = 2;
        } else if (Fen[i] == 'N') {
            tab[k] = 3;
        } else if (Fen[i] == 'B') {
            tab[k] = 4;
        } else if (Fen[i] == 'Q') {
            tab[k] = 5;
        } else if (Fen[i] == 'K') {
            tab[k] = 6;
        } else if (Fen[i] == 'P') {
            tab[k] = 1;
        } else if (Fen[i] == 'r') {
            tab[k] = 9;
        } else if (Fen[i] == 'n') {
            tab[k] = 10;
        } else if (Fen[i] == 'b') {
            tab[k] = 11;
        } else if (Fen[i] == 'q') {
            tab[k] = 12;
        } else if (Fen[i] == 'k') {
            tab[k] = 13;
        } else if (Fen[i] == 'p') {
            tab[k] = 8;
        } else if (Fen[i] == '/') {
            k--;
        } else if (Fen[i]>48&&Fen[i]<57) {
            for (int j = Fen[i]%48; j>0; j--) {    
                tab[k] = 7;
                k++;
            }
            k--;
        } else if (Fen[i] == ' ') {

        } else if (Fen[44] == 'w') {
            tab[64] = 0;
        } else if (Fen[44] == 'b') {
            tab[64] = 7;
        } else if (Fen[i] == 'K' && Fen[i+1] == 'Q') {
            tab[65]++;
        } else if (Fen[i] == 'k' && Fen[i+1] == 'q') {
            tab[65] = tab[65]+2;
        }
        k++;
        
    }
}

void drawCircle(int X, int Y, int radius) 
{
    for (int x = X - radius; x <= X + radius; x++) {
        for (int y = Y - radius; y <= Y + radius; y++) {
            if (pow(x - X, 2) + pow(y - Y, 2) <= pow(radius, 2)) {
                SDL_RenderDrawPoint(renderer, x, y);
            }
        }
    }
}

bool Check(int* tab) 
{
    int KL, ans = 0;
    if (tab[64] == 7) {
        for (int i = 0; i<64; i++) {
            if (tab[i] == 6) {
                KL = i;
            }
        }
    } else if (tab[64] == 0) {
        for (int i = 0; i<64; i++) {
            if (tab[i] == 13) {
                KL = i;
            }
        }
    }
    for (int i = 1; i<64; i++) {
        /// -->
        if (i<8) {
            if ((KL+i)>=0 && (KL+i)<64 && ((KL+i%8)%8)!=0 &&(tab[KL+i]-7)*(tab[KL]-7)<0) {
                if (tab[KL+i] == 1+tab[64] || tab[KL+i] == 3+tab[64] || tab[KL+i] == 4+tab[64] || (tab[KL+i] == 6+tab[64] && (i%8)>1)) {
                    i = 8;
                } else {
                    ans++;
                    i = 8;
                }
            } else if ((KL+i)>=0 && (KL+i)<64 && (tab[KL+i]-7)*(tab[KL]-7)>0) {
                i = 8;
            } else if (((KL+i%8)%8) == 0) {
                i = 8;
            }
        /// <--
        } else if (i>8 && i<16) {
            if ((KL-i%8)>=0 && (KL-i%8)<64 && ((KL-i%8)%8)!=7 && (tab[KL-i%8]-7)*(tab[KL]-7)<0) {
                if (tab[KL-i%8] == 1+tab[64] || tab[KL-i%8] == 3+tab[64] || tab[KL-i%8] == 4+tab[64] || (tab[KL-i%8] == 6+tab[64] && (i%8)>1)) {
                    i = 16;
                } else {
                    ans++;
                    i = 16;
                }
            } else if ((KL-i%8)>=0 && (KL-i%8)<64 && (tab[KL-i%8]-7)*(tab[KL]-7)>0) {
                i = 16;
            } else if (((KL-i%8)%8) == 7){
                i = 16;
            }
        /// | 
        /// v
        } else if (i>16 && i<24) {
            if ((KL+(i%8)*8)>=0 && (KL+(i%8)*8)<64 && (tab[KL+(i%8)*8]-7)*(tab[KL]-7)<0) {
                if (tab[KL+(i%8)*8] == 1+tab[64] || tab[KL+(i%8)*8] == 3+tab[64] || tab[KL+(i%8)*8] == 4+tab[64] || (tab[KL+(i%8)*8] == 6+tab[64] && (i%8)>1)) {
                    i = 24;
                } else {
                    ans++;
                    i = 24;
                }
            } else if ((KL+(i%8)*8)>=0 && (KL+(i%8)*8)<64 && (tab[KL+(i%8)*8]-7)*(tab[KL]-7)>0) {
                i = 24;
            }
        /// ^
        /// |
        } else if (i>24 && i<32) {  
            if ((KL-(i%8)*8)>=0 && (KL-(i%8)*8)<64 && (tab[KL-(i%8)*8]-7)*(tab[KL]-7)<0) {
                if (tab[KL-(i%8)*8] == 1+tab[64] || tab[KL-(i%8)*8] == 3+tab[64] || tab[KL-(i%8)*8] == 4+tab[64] || (tab[KL-(i%8)*8] == 6+tab[64] && (i%8)>1)) {
                    i = 32;
                } else {
                    ans++;
                    i = 32;
                }
            } else if ((KL-(i%8)*8)>=0 && (KL-(i%8)*8)<64 && (tab[KL-(i%8)*8]-7)*(tab[KL]-7)>0) {
                i = 32;
            }
        } else if (i>32 && i<40) {
            if ((KL+(i%8)*8+i%8)>=0 && ((KL+(i%8)*8+i%8)%8)!=0 && (KL+(i%8)*8+i%8)<64 && (tab[KL+(i%8)*8+i%8]-7)*(tab[KL]-7)<0) {
                if (tab[KL+(i%8)*8+i%8] == 8 || (tab[KL+(i%8)*8+i%8] == 1 && (i%8) != 1) || tab[KL+(i%8)*8+i%8] == 2+tab[64] || tab[KL+(i%8)*8+i%8] == 3+tab[64] || (tab[KL+(i%8)*8+i%8] == 6+tab[64] && (i%8)>1)) {
                    i = 40;
                } else {
                    ans++;
                    i = 40;
                }
                
            } else if ((KL+(i%8)*8+i%8)>=0 && (KL+(i%8)*8+i%8)<64 && (tab[KL+(i%8)*8+i%8]-7)*(tab[KL]-7)>0) {
                i = 40;
            } else if (((KL+(i%8)*8+i%8)%8) == 0) {
                i = 40;
            }
        } else if (i>40 && i<48) {
            if ((KL+(i%8)*8-i%8)>=0 && ((KL+(i%8)*8-i%8)%8)!=7 && (KL+(i%8)*8-i%8)<64 && (tab[KL+(i%8)*8-i%8]-7)*(tab[KL]-7)<0) {
                if (tab[KL+(i%8)*8-i%8] == 8 || (tab[KL+(i%8)*8-i%8] == 1 && (i%8) != 1) || tab[KL+(i%8)*8-i%8] == 2+tab[64] || tab[KL+(i%8)*8-i%8] == 3+tab[64] || (tab[KL+(i%8)*8-i%8] == 6+tab[64] && (i%8)>1)) {
                    i = 48;
                } else {
                    ans++;
                    i = 48;
                }
            } else if ((KL+(i%8)*8-i%8)>=0 && (KL+(i%8)*8-i%8)<64 && (tab[KL+(i%8)*8-i%8]-7)*(tab[KL]-7)>0) {
                i = 48;
            } else if (((KL+(i%8)*8-i%8)%8) == 7) {
                i = 48;
            }
        } else if (i>48 && i<56) {
            if ((KL-(i%8)*8+i%8)>=0 && ((KL-(i%8)*8+i%8)%8)!=0 && (KL-(i%8)*8+i%8)<64 && (tab[KL-(i%8)*8+i%8]-7)*(tab[KL]-7)<0) {
                if (tab[KL-(i%8)*8+i%8] == 1 || (tab[KL-(i%8)*8+i%8] == 8 && (i%8) != 1) || tab[KL-(i%8)*8+i%8] == 2+tab[64] || tab[KL-(i%8)*8+i%8] == 3+tab[64] || (tab[KL-(i%8)*8+i%8] == 6+tab[64] && (i%8)>1)) {
                    i = 56;
                } else {
                    ans++;
                    i = 56;
                }
            } else if ((KL-(i%8)*8+i%8)>=0 && (KL-(i%8)*8+i%8)<64 && (tab[KL-(i%8)*8+i%8]-7)*(tab[KL]-7)>0) {
                i = 56;
            } else if (((KL-(i%8)*8+i%8)%8) == 0) {
                i = 56;
            }
        } else if (i>56 && i<64) {
            if ((KL-(i%8)*8-i%8)>=0 && ((KL-(i%8)*8-i%8)%8)!=0 &&(KL-(i%8)*8-i%8)<64 &&(tab[KL-(i%8)*8-i%8]-7)*(tab[KL]-7)<0) {
                if (tab[KL-(i%8)*8-i%8] == 1 || (tab[KL-(i%8)*8-i%8] == 8 && (i%8) != 1) || tab[KL-(i%8)*8-i%8] == 2+tab[64] || tab[KL-(i%8)*8-i%8] == 3+tab[64] || (tab[KL-(i%8)*8-i%8] == 6+tab[64] && (i%8)>1)) {
                    i = 64;
                } else {
                    ans++;
                    i = 64;
                }

            } else if ((KL-(i%8)*8-i%8)>=0 && (KL-(i%8)*8-i%8)<64 && (tab[KL-(i%8)*8-i%8]-7)*(tab[KL]-7)>0) {
                i = 64;
            } else if (((KL-(i%8)*8-i%8)%8) == 7) {
                i = 64;
            }
        }
    }   
    ///knight check
    if (KL-17>=0 && KL-17<64 && (tab[KL-17]-7)*(tab[KL]-7)<0 && tab[KL-17] == 3+tab[64]) {
        ans++;
    }
    if (KL-15>=0 && KL-15<64 && (tab[KL-15]-7)*(tab[KL]-7)<0 && tab[KL-15] == 3+tab[64]) {
        ans++;
    }
    if (KL-10>=0 && KL-10<64 && (tab[KL-10]-7)*(tab[KL]-7)<0 && tab[KL-10] == 3+tab[64]) {
        ans++;
    }
    if (KL-6>=0 && KL-6<64 && (tab[KL-6]-7)*(tab[KL]-7)<0 && tab[KL-6] == 3+tab[64]) {
        ans++;
    }
    if (KL+6>=0 && KL+6<64 && (tab[KL+6]-7)*(tab[KL]-7)<0 && tab[KL+6] == 3+tab[64]) {
        ans++;
    }
    if (KL+10>=0 && KL+10<64 && (tab[KL+10]-7)*(tab[KL]-7)<0 && tab[KL+10] == 3+tab[64]) {
        ans++;
    }
    if (KL+15>=0 && KL+15<64 && (tab[KL+15]-7)*(tab[KL]-7)<0 && tab[KL+15] == 3+tab[64]) {
        ans++;
    }
    if (KL+17>=0 && KL+17<64 && (tab[KL+17]-7)*(tab[KL]-7)<0 && tab[KL+15] == 3+tab[64]) {
        ans++;
    }

    return ans == 0 ;
}

void Rock(int* tab, int* M, int x, int y, int* RAM)
{
    int p = tab[y*8+x];
    if (p<7) {
        if (tab[61] == 7 && tab[62] == 7 && tab[60] == 6 && tab[63] == 2 && RAM[1] != 1 && Check(tab) == 1) {
            tab[64] = 7;
            tab[61] = 6;
            if (Check(tab) == 1) {
                M[30] = 62;
                M[32] = 63;
                M[33] = 61;
            }
            tab[61] = 7;
            tab[64] = 0;
        } 
        
        if (tab[57] == 7 && tab[58] == 7 && tab[59] == 7 && tab[60] == 6 && tab[56] == 2 && RAM[0] != 1 && Check(tab) == 1) {
            tab[64] = 7;
            tab[59] = 6;
            tab[60] = 7;
            if (Check(tab) == 1) {
                M[31] = 58;
                M[34] = 56;
                M[35] = 59;
            }
            tab[60] = 6;
            tab[59] = 7;
            tab[64] = 0;
        }
    } else if (p>7) {
        if (tab[5] == 7 && tab[6] == 7 && tab[4] == 13 && tab[7] == 9 && RAM[3] != 1 && Check(tab) == 1) {
            tab[64] = 0;
            tab[5] = 13;
            if (Check(tab) == 1) {
                M[30] = 6;
                M[32] = 7;
                M[33] = 5;
            }
            tab[5] = 7;
            tab[64] = 7;
        } 
        if (tab[1] == 7 && tab[2] == 7 && tab[3] == 7 && tab[4] == 13 && tab[0] == 9 && RAM[2] != 1 && Check(tab) == 1) {
            tab[64] = 0;
            tab[3] = 13;
            tab[4] = 7;
            if (Check(tab) == 1) {
                M[31] = 2;
                M[34] = 0;
                M[35] = 3;
            }
            tab[4] = 13;
            tab[3] = 7;    
            tab[64] = 7;
        }
    }  
}

void add(int* tab, int x, int y, int tx, int ty) 
{
    tab[ty*8+tx] = tab[y*8+x];
    tab[y*8+x] = 7;
}

void LegalM(int* tab,int x, int y, int* M, int* RAM)
{
    int temp = 1, index = 0, x2 = x, y2 = y;
    int p = tab[y*8+x]/*Piece*/;

    if (p == 1) {   
        /*Wpond*/
        if (y == 6 && tab[x+(y-1)*8]==7 && tab[x+(y-2)*8]==7) {
            M[2] = (y-2)*8+x;
        }
        if (tab[x+(y-1)*8]==7) {
            M[1] = (y-1)*8+x;
        }
        if (tab[x+1+(y-1)*8]>7 && x+1<8 && y-1>=0 && x+1+(y-1)*8<64 && x+1+(y-1)*8>=0 ) {
            M[3] = x+1+(y-1)*8;
        }
        if (tab[x-1+(y-1)*8]>7 && x-1>=0 && y-1>=0 && x-1+(y-1)*8<64 && x-1+(y-1)*8>=0) {
            M[4] = x-1+(y-1)*8;
        }
        if ((x+(y-1)*8-1 == RAM[4] || x+(y-1)*8+1 == RAM[4]) && tab[RAM[4]+8] == 8) {
            M[5] = RAM[4];
        }
    } else if (p == 2 || p == 9) {
        /*Rock*/
        for (int i = 1;i<32; i++) {
            if (i<8) {
                if (x-i>=0 && ((tab[y*8+x-i]-7)*(p-7)<0 || tab[y*8+x-i] == 7)) {
                    M[temp] = y*8+x-i;
                    temp++;
                    if ((tab[y*8+x-i]-7)*(p-7)<0) {
                        i = 8;
                    }
                } else {
                    i = 8;
                }
            } else if (i>8 && i<16) {
                if (x+i%8<8 && ((tab[y*8+x+i%8]-7)*(p-7)<0 || tab[y*8+x+i%8] == 7)) {
                    M[temp] = y*8+x+i%8;
                    temp++;
                    if ((tab[y*8+x+i%8]-7)*(p-7)<0) {
                        i = 16;
                    }
                } else {
                    i = 16;
                }
            } else if (i>16 && i<24) {
                if (y-i%8>=0 && ((tab[(y-i%8)*8+x]-7)*(p-7)<0 || tab[(y-i%8)*8+x] == 7)) {
                    M[temp] = (y-i%8)*8+x;
                    temp++;
                    if ((tab[(y-i%8)*8+x]-7)*(p-7)<0) {
                        i = 24;
                    }
                } else {
                    i = 24;
                }
            } else if (i>24 && i<32) {
                if (y+i%8<8 && ((tab[(y+i%8)*8+x]-7)*(p-7)<0 || tab[(y+i%8)*8+x] == 7)) {
                    M[temp] = (y+i%8)*8+x;
                    temp++;
                    if ((tab[(y+i%8)*8+x]-7)*(p-7)<0) {
                        i = 32;
                    }
                } else {
                    i = 32;
                }
            }
        }
    } else if (p == 4 || p == 11) {
        /*Bishop*/
        int j = 1, a = 1, b = 1, c = 1, d = 1;
        for (int i = 1; i<8; i++) {
            if (x+i<8 && y+j<8 && x+i+(y+j)*8<64 && x+i+(y+j)*8>=0 && ((tab[x+i+(y+j)*8]-7)*(p-7)<0 || tab[x+i+(y+j)*8] == 7) && a == 1) {
                M[temp] = x+i+(y+j)*8;
                temp++;
                if ((tab[x+i+(y+j)*8]-7)*(p-7)<0) {
                    a = 0;
                }
            } else {
                a = 0;
            }
            if (x-i>=0 && y-j>=0 && x-i+(y-j)*8<64 && x-i+(y-j)*8>=0 && ((tab[x-i+(y-j)*8]-7)*(p-7)<0 || tab[x-i+(y-j)*8] == 7) && b == 1) {
                M[temp] = x-i+(y-j)*8;
                temp++;
                if ((tab[x-i+(y-j)*8]-7)*(p-7)<0) {
                    b = 0;
                }
            } else {
                b = 0;
            }
            if (x+i<8 && y-j>=0 && x+i+(y-j)*8<64 && x+i+(y-j)*8>=0 && ((tab[x+i+(y-j)*8]-7)*(p-7)<0 || tab[x+i+(y-j)*8] == 7) && c == 1) {
                M[temp] = x+i+(y-j)*8;
                temp++;
                if ((tab[x+i+(y-j)*8]-7)*(p-7)<0) {
                    c = 0;
                }
            } else {
                c = 0;
            }
            if (x-i>=0 && y+j<8 && x-i+(y+j)*8<64 && x-i+(y+j)*8>=0 && ((tab[x-i+(y+j)*8]-7)*(p-7)<0 || tab[x-i+(y+j)*8] == 7) && d == 1) {
                M[temp] = x-i+(y+j)*8;
                temp++;
                if ((tab[x-i+(y+j)*8]-7)*(p-7)<0) {
                    d = 0;
                }
            } else {
                d = 0;
            }
            j++;
        }
    } else if (p == 3 || p == 10) {
        /*Knight*/
        if ((x-1>=0) && (y-2>=0) && x-1+(y-2)*8<64 && x-1+(y-2)*8>=0 && ((tab[x-1+(y-2)*8]-7)*(p-7)<0 || tab[x-1+(y-2)*8] == 7)) {
            M[1] = x-1+(y-2)*8;
        }
        if ((x+1<8) && (y-2>=0) && x+1+(y-2)*8<64 && x+1+(y-2)*8>=0 && ((tab[x+1+(y-2)*8]-7)*(p-7)<0 || tab[x+1+(y-2)*8] == 7)) {
            M[2] = x+1+(y-2)*8;
        }
        if ((x-1>=0) && (y+2<8) && x-1+(y+2)*8<64 && x-1+(y+2)*8>=0 && ((tab[x-1+(y+2)*8]-7)*(p-7)<0 || tab[x-1+(y+2)*8] == 7)) {
            M[3] = x-1+(y+2)*8;
        }
        if ((x+1<8) && (y+2<8) && x+1+(y+2)*8<64 && x+1+(y+2)*8>=0 && ((tab[x+1+(y+2)*8]-7)*(p-7)<0 || tab[x+1+(y+2)*8] == 7)) {
            M[4] = x+1+(y+2)*8;
        }
        if ((x-2>=0) && (y-1>=0) && x-2+(y-1)*8<64 && x-2+(y-1)*8>=0 && ((tab[x-2+(y-1)*8]-7)*(p-7)<0 || tab[x-2+(y-1)*8] == 7)) {
            M[5] = x-2+(y-1)*8;
        }
        if ((x-2>=0) && (y+1<8) && x-2+(y+1)*8<64 && x-2+(y+1)*8>=0 && ((tab[x-2+(y+1)*8]-7)*(p-7)<0 || tab[x-2+(y+1)*8] == 7)) {
            M[7] = x-2+(y+1)*8;
        }
        if ((x+2<8) && (y-1>0) && x+2+(y-1)*8<64 && x+2+(y-1)*8>=0 && ((tab[x+2+(y-1)*8]-7)*(p-7)<0 || tab[x+2+(y-1)*8] == 7)) {
            M[7] = x+2+(y-1)*8;
        }
        if ((x+2<8) && (y+1<8) && x+2+(y+1)*8<64 && x+2+(y+1)*8>=0 && ((tab[x+2+(y+1)*8]-7)*(p-7)<0 || tab[x+2+(y+1)*8] == 7)) {
            M[8] = x+2+(y+1)*8;
        }
    } else if (p == 5 || p == 12) {
        /*Queen*/
        for (int i = 1;i<32; i++) {
            if (i<8) {
                if (x-i>=0 && ((tab[y*8+x-i]-7)*(p-7)<0 || tab[y*8+x-i] == 7)) {
                    M[temp] = y*8+x-i;
                    temp++;
                    if ((tab[y*8+x-i]-7)*(p-7)<0) {
                        i = 8;
                    }
                } else {
                    i = 8;
                }
            } else if (i>8 && i<16) {
                if (x+i%8<8 && ((tab[y*8+x+i%8]-7)*(p-7)<0 || tab[y*8+x+i%8] == 7)) {
                    M[temp] = y*8+x+i%8;
                    temp++;
                    if ((tab[y*8+x+i%8]-7)*(p-7)<0) {
                        i = 16;
                    }
                } else {
                    i = 16;
                }
            } else if (i>16 && i<24) {
                if (y-i%8>=0 && ((tab[(y-i%8)*8+x]-7)*(p-7)<0 || tab[(y-i%8)*8+x] == 7)) {
                    M[temp] = (y-i%8)*8+x;
                    temp++;
                    if ((tab[(y-i%8)*8+x]-7)*(p-7)<0) {
                        i = 24;
                    }
                } else {
                    i = 24;
                }
            } else if (i>24 && i<32) {
                if (y+i%8<8 && ((tab[(y+i%8)*8+x]-7)*(p-7)<0 || tab[(y+i%8)*8+x] == 7)) {
                    M[temp] = (y+i%8)*8+x;
                    temp++;
                    if ((tab[(y+i%8)*8+x]-7)*(p-7)<0) {
                        i = 32;
                    }
                } else {
                    i = 32;
                }
            }
        }
        int j = 1, a = 1, b = 1, c = 1, d = 1;
        for (int i = 1; i<8; i++) {
            if (x+i<8 && y+j<8 && x+i+(y+j)*8<64 && x+i+(y+j)*8>=0 && ((tab[x+i+(y+j)*8]-7)*(p-7)<0 || tab[x+i+(y+j)*8] == 7) && a == 1) {
                M[temp] = x+i+(y+j)*8;
                temp++;
                if ((tab[x+i+(y+j)*8]-7)*(p-7)<0) {
                    a = 0;
                }
            } else {
                a = 0;
            }
            if (x-i>=0 && y-j>=0 && x-i+(y-j)*8<64 && x-i+(y-j)*8>=0 && ((tab[x-i+(y-j)*8]-7)*(p-7)<0 || tab[x-i+(y-j)*8] == 7) && b == 1) {
                M[temp] = x-i+(y-j)*8;
                temp++;
                if ((tab[x-i+(y-j)*8]-7)*(p-7)<0) {
                    b = 0;
                }
            } else {
                b = 0;
            }
            if (x+i<8 && y-j>=0 && x+i+(y-j)*8<64 && x+i+(y-j)*8>=0 && ((tab[x+i+(y-j)*8]-7)*(p-7)<0 || tab[x+i+(y-j)*8] == 7) && c == 1) {
                M[temp] = x+i+(y-j)*8;
                temp++;
                if ((tab[x+i+(y-j)*8]-7)*(p-7)<0) {
                    c = 0;
                }
            } else {
                c = 0;
            }
            if (x-i>=0 && y+j<8 && x-i+(y+j)*8<64 && x-i+(y+j)*8>=0 && ((tab[x-i+(y+j)*8]-7)*(p-7)<0 || tab[x-i+(y+j)*8] == 7) && d == 1) {
                M[temp] = x-i+(y+j)*8;
                temp++;
                if ((tab[x-i+(y+j)*8]-7)*(p-7)<0) {
                    d = 0;
                }
            } else {
                d = 0;
            }
            j++;
        }
    } else if (p == 6 || p == 13) {
        /*King*/
        if (x+1<8 && ((tab[y*8+x+1]-7)*(p-7)<0 || tab[y*8+x+1] == 7)) {
            M[1] = y*8+x+1;
        }
        if (x-1>=0 && ((tab[y*8+x-1]-7)*(p-7)<0 || tab[y*8+x-1] == 7)) {
            M[2] = y*8+x-1;
        }
        if (y+1<8 && ((tab[(y+1)*8+x]-7)*(p-7)<0 || tab[(y+1)*8+x] == 7)) {
            M[3] = (y+1)*8+x;
        }
        if (y-1>=0 && ((tab[(y-1)*8+x]-7)*(p-7)<0 || tab[(y-1)*8+x] == 7)) {
            M[4] = (y-1)*8+x;
        }
        if (y-1>=0 && x+1<8 && (y-1)*8+x+1<64 && (y-1)*8+x+1<64>=0 && ((tab[(y-1)*8+x+1]-7)*(p-7)<0 || tab[(y-1)*8+x+1] == 7)) {
            M[5] = (y-1)*8+x+1;
        }
        if (y-1>=0 && x-1>=0 && (y-1)*8+x-1<64 && (y-1)*8+x-1>=0 && ((tab[(y-1)*8+x-1]-7)*(p-7)<0 || tab[(y-1)*8+x-1] == 7)) {
            M[6] = (y-1)*8+x-1;
        }
        if (y+1<8 && x+1<8 && (y+1)*8+x+1<64 && (y+1)*8+x+1>=0 && ((tab[(y+1)*8+x+1]-7)*(p-7)<0 || tab[(y+1)*8+x+1] == 7)) {
            M[7] = (y+1)*8+x+1;
        }
        if (y+1<8 && x-1>=0 && (y+1)*8+x-1<64 && (y+1)*8+x-1>=0 && ((tab[(y+1)*8+x-1]-7)*(p-7)<0 || tab[(y+1)*8+x-1] == 7)) {
            M[8] = (y+1)*8+x-1;
        }
        Rock(tab,M,x,y,RAM);
    } else if (p == 8) {
        /*Bpond*/
        if (y == 1 && tab[x+(y+1)*8]==7 && tab[x+(y+2)*8]==7) {
            M[2] = (y+2)*8+x;
        } 
        if (tab[x+(y+1)*8]==7) {
            M[1] = (y+1)*8+x;
        }
        if (tab[x+1+(y+1)*8]<7 && tab[x+1+(y+1)*8]>0 && x+1<8 && y+1<8 && x+1+(y+1)*8<64 && x+1+(y+1)*8>=0 ) {
            M[3] = x+1+(y+1)*8;
        }
        if (tab[x-1+(y+1)*8]<7 && tab[x-1+(y+1)*8]>0 && x-1>=0 && y+1<8 && x-1+(y+1)*8<64 && x-1+(y+1)*8>=0 ) {
            M[4] = x-1+(y+1)*8;
        }
        if ((x+(y+1)*8-1 == RAM[4] || x+(y+1)*8+1 == RAM[4]) && tab[RAM[4]-8] == 1) {
            M[5] = RAM[4];
        }
    } 
    int* Tabtst;
    Tabtst = (int*)malloc(sizeof(int)*65);

    for (int i = 0; i<32; i++) {
        if(M[i]!=-1) {
            for (int j = 0; j<64; j++) {
                Tabtst[j] = tab[j];
            }      
            if (p<7) {
                Tabtst[64] = 7;
            } else {
                Tabtst[64] = 0;
            } 

            add(Tabtst,x2,y2,(M[i]%8),(M[i]/8));
            if (Check(Tabtst)==0) {
                M[i]=-1;
            }
        
        }
    }
    free(Tabtst);
}

bool CheckMate(int* tab, int* M, int* RAM, int colour) 
{
    int ans = 0;
    for(int i = 0; i<64; i++) {
        if((tab[i]-7)*(colour-7)>0) {
            LegalM(tab,(i%8),(i/8),M,RAM);
            for (int j = 0; j<32; j++) {
                if (M[j]!=-1) {
                    ans++;
                    goto end;
                }
            }
        }
    }
    end:
    for (int j = 0; j<34; j++) { 
        M[j] = -1;
    }
    return ans==0;
}

void affichage(int* tab, int* M)
{
    
    SDL_Rect rectangle;
    SDL_Rect square;
    int x, y;
    for (int i = 0; i<8; i++) {
        for (int j = 0; j<8; j++) {
            if ((j+i) % 2 == 0) {
                SDL_SetRenderDrawColor(renderer, 104, 114, 133, SDL_ALPHA_OPAQUE);
            } else {
                SDL_SetRenderDrawColor(renderer, 42, 49, 63, SDL_ALPHA_OPAQUE);
            }
            square.x = i*width/8;
            square.y = j*height/8;
            square.h = width/8;
            square.w = height/8;
            SDL_RenderFillRect(renderer, &square);
        }
    }

    for (int i = 0 ; i<64; i++) {
        SDL_Texture* texture = NULL; 
        SDL_Surface* image = NULL;
        if(tab[i]==0) {

        } else if (tab[i]==1) {
            image = SDL_LoadBMP("Chess/WPond.bmp");
        } else if (tab[i]==2) {
            image = SDL_LoadBMP("Chess/WRock.bmp");
        } else if (tab[i]==3) {
            image = SDL_LoadBMP("Chess/WKnight.bmp");
        } else if (tab[i]==4) {
            image = SDL_LoadBMP("Chess/WBishop.bmp");
        } else if (tab[i]==5) {
            image = SDL_LoadBMP("Chess/WQueen.bmp");
        } else if (tab[i]==6) {
            image = SDL_LoadBMP("Chess/WKing.bmp");
        } else if (tab[i]==8) {
            image = SDL_LoadBMP("Chess/BPond.bmp");
        } else if (tab[i]==9) {
            image = SDL_LoadBMP("Chess/BRock.bmp");
        } else if (tab[i]==10) {
            image = SDL_LoadBMP("Chess/BKnight.bmp");
        } else if (tab[i]==11) {
            image = SDL_LoadBMP("Chess/BBishop.bmp");
        } else if (tab[i]==12) {
            image = SDL_LoadBMP("Chess/BQueen.bmp");
        } else if (tab[i]==13) {
            image = SDL_LoadBMP("Chess/BKing.bmp");
        }
        rectangle.x = (width/8)*(i%8);
        rectangle.y = (height/8)*(i/8);
        rectangle.w = width/8;
        rectangle.h = height/8;
        texture = SDL_CreateTextureFromSurface(renderer, image);
        SDL_RenderCopy(renderer, texture, NULL, &rectangle); 
        SDL_DestroyTexture(texture);
        if (image != NULL) {
            SDL_FreeSurface(image);
        }
        image = NULL;
    }
    SDL_SetRenderDrawColor(renderer, 160, 160, 160, SDL_ALPHA_TRANSPARENT);
    for (int i = 0; i<32; i++) {
        if (M[i]!=-1) {
            drawCircle((M[i]%8)*(width/8)+(width/16),(M[i]/8)*(height/8)+(height/16),10);
        }
    }
    SDL_RenderPresent(renderer);
    
}

bool validM(int tx, int ty, int* M)
{
    int a = 0;
    for (int i = 0; i<32; i++) {
        if (M[i] == tx+(ty*8)) {
            a++;
        }
    }
    return a!=0;
}

void promotion(int* tab, int* M, int txypos) 
{
    if (((txypos)>=0 && (txypos)<7 && tab[txypos] == 1)||((txypos)<=63 && (txypos)>=56) && tab[txypos] == 8) {
        int running = 1, x, y, xypos, colour;
        SDL_Rect rect;
        SDL_Event Event;
        rect.x = 0;
        rect.w = width/2;
        rect.h = height/8;
        SDL_SetRenderDrawColor(renderer,255,255,255,SDL_ALPHA_OPAQUE);
        if (txypos<56) {
            rect.y = 0;    
            colour = 0;
            SDL_RenderFillRect(renderer,&rect);
            rect.w = width/8;
            for (int i = 0; i<4; i++) {
                SDL_Texture* texture = NULL; 
                SDL_Surface* image = NULL;
                if (i == 0) {
                    image = SDL_LoadBMP("Chess/WBishop.bmp");
                } else if (i == 1) {
                    image = SDL_LoadBMP("Chess/WKnight.bmp");
                } else if (i == 2) {
                    image = image = SDL_LoadBMP("Chess/WRock.bmp");
                } else if (i == 3) {
                    image = SDL_LoadBMP("Chess/WQueen.bmp");
                }
                rect.x = i*(width/8);
                texture = SDL_CreateTextureFromSurface(renderer, image);
                SDL_RenderCopy(renderer, texture, NULL, &rect); 
                SDL_DestroyTexture(texture);
            }
        } else {
            rect.y = 700;
            colour = 7;
            SDL_RenderFillRect(renderer,&rect);
            rect.w = width/8;
            for (int i = 0; i<4; i++) {
                SDL_Texture* texture = NULL; 
                SDL_Surface* image = NULL;
                if (i == 0) {
                    image = SDL_LoadBMP("Chess/BBishop.bmp");
                } else if (i == 1) {
                    image = SDL_LoadBMP("Chess/BKnight.bmp");
                } else if (i == 2) {
                    image = image = SDL_LoadBMP("Chess/BRock.bmp");
                } else if (i == 3) {
                    image = SDL_LoadBMP("Chess/BQueen.bmp");
                }
                rect.x = i*(width/8);
                texture = SDL_CreateTextureFromSurface(renderer, image);
                SDL_RenderCopy(renderer, texture, NULL, &rect); 
                SDL_DestroyTexture(texture);
            }
        }
        SDL_RenderPresent(renderer);
        while(running) {
            f:
            while (SDL_PollEvent(&Event)) {
                if (Event.type == SDL_QUIT||Event.type == SDL_SCANCODE_ESCAPE) {
                    running = 0;
                }
                if (Event.type == SDL_MOUSEBUTTONDOWN) {
                    SDL_GetMouseState(&x, &y);
                    x = x*8/width;
                    y = y*8/height;
                    xypos = x+(y*8);
                    if(xypos == 0 || xypos == 56) {
                        tab[txypos] = 4 + colour;
                        running = 0;
                    } else if (xypos == 1 || xypos == 57) {
                        tab[txypos] = 3 + colour;
                        running = 0;
                    } else if (xypos == 2 || xypos == 58) {
                        tab[txypos] = 2 + colour;
                        running = 0;
                    } else if (xypos == 3 || xypos == 59) {
                        tab[txypos] = 5 + colour;
                        running = 0;
                    } else {
                        goto f;
                    }
                }
            }
        }
    affichage(tab,M);
    }
}

void Rockmove(int* tab, int* M, int* RAM, int xypos, int txypos)
{
    int  Rockside, Rockmove;
    if (tab[txypos] == 2 || tab[txypos] == 9 || tab[txypos] == 6 || tab[txypos] == 13) {
        if (xypos == 0 && tab[txypos] == 9) {
            RAM[0] = 1;
        } else if (xypos == 7 && tab[txypos] == 9) {
            RAM[1] = 1;
        } else if (xypos == 56 && tab[txypos] == 2) {
            RAM[2] = 1;
        } else if (xypos == 63 && tab[txypos] == 2) {
            RAM[3] = 1;
        } else if (xypos == 60 && tab[txypos] == 6) {
            RAM[0] = 1;
            RAM[1] = 1;
        } else if (xypos == 4 && tab[txypos] == 13) {
            RAM[2] = 1;
            RAM[3] = 1;
        }
    }
    if ((tab[txypos] == 6 || tab[txypos] == 13) && abs((txypos)-(xypos))>1) {
       if ((txypos) > xypos) {
            Rockside = M[32];
            Rockmove = M[33];
        } else {
            Rockside = M[34];
            Rockmove = M[35];
        }
        tab[Rockside] = 7;
        if(tab[txypos] == 6) {
            tab[Rockmove] = 2;
        } else {
            tab[Rockmove] = 9;
        }   
    }
}

void Enpassant(int* tab,int* RAM, int xypos, int txypos)
{
    if ((tab[txypos] == 1 && ((xypos)-(txypos)) == 16)) {
        RAM[4] = (txypos)+8;
    } else if ((tab[txypos] == 8 && ((txypos)-(xypos)) == 16)) {
        RAM[4] = (txypos)-8;
    } else if ((tab[txypos] == 1 && ((xypos)-(txypos))%8 != 0) && RAM[4] == (txypos)) {
        tab[(txypos)+8] = 7;
        RAM[4] = -1;
    } else if ((tab[txypos] == 8 && ((txypos)-(xypos))%8 != 0) && RAM[4] == (txypos)) {
        tab[(txypos)-8] = 7;
        RAM[4] = -1;
    }
}

/*
void test(int* tab, int* M, int* RAM, int depth, int* c) {
  if (depth == 0) {
    (*c)++;
  } else {
    for (int i = 0; i < 64; i++) {
      if ((tab[i] - 7) * (tab[64] - 7) > 0) {
        LegalM(tab, i % 8, i / 8, M, RAM);
        for (int j = 0; j < 32; j++) {
          if (M[j] != -1) {
            int* Tabtst;
            Tabtst = (int*)malloc(sizeof(int) * 65);
            for (int k = 0; k < 64; k++) {
              Tabtst[k] = tab[k];
            }
            add(Tabtst, i % 8, i / 8, M[j] % 8, M[j] / 8);
            Rockmove(Tabtst, M, RAM, (i % 8) + (i / 8) * 8, (M[j] % 8) + (M[j] / 8) * 8);
            Enpassant(Tabtst, RAM, (i % 8) + (i / 8) * 8, (M[j] % 8) + (M[j] / 8) * 8);
            affichage(Tabtst, M);
            test(Tabtst, M, RAM, depth - 1, c);
            (*c)++;
            free(Tabtst);
          }
        }
      }
    }
  }
}
*/

void initrender()
{
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        SDL_ExitWithError("Unable to initialize video");
    }
    window = SDL_CreateWindow("Chess", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, 0);
    renderer = SDL_CreateRenderer(window, -1, 0);
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "2");
}

void initsound()
{
    SDL_Init(SDL_INIT_AUDIO);
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        printf("Erreur d'initialisation de SDL_mixer: %s\n", Mix_GetError());
        exit(1);
    }
    movingsound = Mix_LoadWAV("Chess/chess.wav");
    if (!movingsound) {
        printf("Erreur de chargement du fichier son : %s\n", Mix_GetError());
        exit(1);
    }
    initgamesound = Mix_LoadWAV("Chess/chessstart.wav");
    if (!initgamesound) {
        printf("Erreur de chargement du fichier son : %s\n", Mix_GetError());
        exit(1);
    }
    checksound = Mix_LoadWAV("Chess/check.wav");
    if (!checksound) {
        printf("Erreur de chargement du fichier son : %s\n", Mix_GetError());
        exit(1);
    }
    takesound = Mix_LoadWAV("Chess/take.wav");
    if (!takesound) {
        printf("Erreur de chargement du fichier son : %s\n", Mix_GetError());
        exit(1);
    }
    rocksound = Mix_LoadWAV("Chess/rock.wav");
    if (!rocksound) {
        printf("Erreur de chargement du fichier son : %s\n", Mix_GetError());
        exit(1);
    }
    endgamesound = Mix_LoadWAV("Chess/endgame.wav");
    if (!endgamesound) {
        printf("Erreur de chargement du fichier son : %s\n", Mix_GetError());
        exit(1);
    }
}

void initmainmenu()
{
    SDL_Rect rectangle;
    SDL_Texture* texture = NULL; 
    SDL_Surface* image = NULL;
    image = SDL_LoadBMP("Chess/chessscreen.bmp");
    rectangle.w = width;
    rectangle.h = height;
    rectangle.x = 0;
    rectangle.y = 0;
    texture = SDL_CreateTextureFromSurface(renderer, image);
    SDL_RenderCopy(renderer, texture, NULL, &rectangle); 

    image = SDL_LoadBMP("Chess/startbutton.bmp");
    rectangle.w = width/4;
    rectangle.h = height/4;
    rectangle.x = (width-rectangle.w)/2;
    rectangle.y = (height-rectangle.h)/2;
    texture = SDL_CreateTextureFromSurface(renderer, image);
    SDL_RenderCopy(renderer, texture, NULL, &rectangle); 

    SDL_DestroyTexture(texture);
    if (image != NULL) {
            SDL_FreeSurface(image);
        }
    image = NULL;
    SDL_RenderPresent(renderer);
}

void closeevrything()
{
    Mix_FreeChunk(movingsound);
    Mix_FreeChunk(initgamesound);
    Mix_FreeChunk(checksound);
    Mix_FreeChunk(takesound);
    Mix_FreeChunk(rocksound);
    Mix_FreeChunk(endgamesound);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    Mix_CloseAudio();
    SDL_Quit();
}

void initboard(char* Fen, int* tab) 
{
    initrender();
    mainmenu:
    SDL_Event Event;
    int x, y, tx, ty, xypos, txypos, running = 1, ranning, temp = 1, intrun = 1, Evall, exchange;
    FEN(Fen,tab);
    int* M;
    int* RAM;
    RAM = (int*)malloc(sizeof(int)*5);
    M = (int*)malloc(sizeof(int)*36);
    for (int i = 0 ; i<36; i++) {
        M[i] = -1;
    }
    initsound();
    initmainmenu();
    while (intrun) {
        while (SDL_PollEvent(&Event)) {
            if (Event.type == SDL_QUIT||Event.type == SDL_SCANCODE_ESCAPE) {
                intrun = 0;
                running = 0;
            } 
            if (Event.type == SDL_MOUSEBUTTONDOWN) {
                SDL_GetMouseState(&x,&y);
                if (x>= (width-width/4)/2 && x<=((width-width/4)/2)+width && y>=(height-height/4)/2 && y<= ((width-width/4)/2)+height) {
                    intrun = 0;
                    Mix_PlayChannel(-1, initgamesound, 0);
                } else {
                    Event.type = SDL_MOUSEBUTTONUP;
                }
            }
        }
    }





    affichage(tab,M);
    while (running) {
        ranning = 1;
        emptychoice:
        while (SDL_PollEvent(&Event)) {
            if (Event.type == SDL_QUIT||Event.type == SDL_SCANCODE_ESCAPE) {
                running = 0;
            }
            if (Event.type == SDL_MOUSEBUTTONDOWN) {
                SDL_GetMouseState(&x, &y);
                x = x*8/width;
                y = y*8/height;
                xypos = x+(y*8);
                here:
                if (tab[xypos]==7 || (tab[xypos]-7)*(tab[64]-6)<0) {
                    goto emptychoice;
                }
                for (int i = 0 ; i<36; i++) {
                    M[i] = -1;
                }  
                Event.type = SDL_MOUSEBUTTONUP;
                LegalM(tab,x,y,M,RAM);
                affichage(tab,M);
                while(ranning) {
                    while (SDL_PollEvent(&Event)) {
                        if (Event.type == SDL_QUIT||Event.type == SDL_SCANCODE_ESCAPE) {
                            running = 0;
                            ranning = 0;
                        }
                        if (Event.type == SDL_MOUSEBUTTONDOWN) {
                            SDL_GetMouseState(&tx, &ty);
                            tx = tx*8/width;
                            ty = ty*8/height;
                            txypos = tx+(ty*8);
                            if ((tx!=x || ty!=y) && validM(tx,ty,M) == true) {
                                ranning = 0;
                            } else if (tab[txypos]!=7 && (tab[txypos]-7)*(tab[xypos]-7)>0){
                                x = tx;
                                y = ty;
                                xypos = txypos;
                                goto here;
                            }
                        }
                    }
                    
                }  
            } 
        }
        if (validM(tx,ty,M) == true) {
            SDL_Delay(100);
            add(tab,x,y,tx,ty);
            exchange = Evall;
            Evall = Eval(tab);
            if (Evall != exchange) {
                Mix_PlayChannel(-1, takesound, 0);
            } else if (Check(tab) == false) {
                Mix_PlayChannel(-1, checksound, 0);
            } else if ((tab[txypos] == 6 || tab[txypos] == 13) && abs((txypos)-(xypos))>1 && abs((txypos)-(xypos))<5) {
                Mix_PlayChannel(-1, rocksound, 0);
            } else {
                Mix_PlayChannel(-1, movingsound, 0);
            }
            
            /// Promotion
            promotion(tab,M,txypos);
            ///Rock/King moove
            Rockmove(tab,M,RAM,xypos,txypos);
            //En-passant
            Enpassant(tab,RAM,xypos,txypos);
        } 
        
        for (int i = 0 ; i<36; i++) {
                M[i] = -1;
        }
        affichage(tab,M);
        if (CheckMate(tab,M,RAM,tab[64]+1)==1) {
            running = 0;
            ranning = 0;
            printf("Checkmate\n");
            Mix_PlayChannel(-1, endgamesound, 0);
            SDL_Delay(3000);
            goto mainmenu;
         }
         if (ranning == 0) {
            tab[64] = tab[64]+7*temp;
            temp = -temp;
        }
    }
    free(M);
    free(RAM);
    free(tab);
    closeevrything();
}

int main() 
{
    int* tab;
    tab = (int*)malloc(sizeof(int)*67);
    /*int* M;
    int* RAM;
    RAM = (int*)malloc(sizeof(int)*5);
    M = (int*)malloc(sizeof(int)*36);
    int c = 0;
    test(tab,M,RAM,1,&c);
    printf("\nGRRRRRRR => %d\n", c);*/

    initboard("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq",tab);


    
    return 0;
}