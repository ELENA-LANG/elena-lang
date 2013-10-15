#include <stdio.h> 
 
int p, i, loopStart = 0; 
int a[1024]; 
char buf[1024]; 
 
void interpret(char *c) 
{  
  if(c == '>') p++; 
  if(c == '<') if(p>0) p--; 
  if(c == '+') a[p]++; 
  if(c == '-') a[p]--; 
  if(c == '.') printf("%c",a[p]); 
  if(c == ',') a[p] = getchar(); 
  if(c == '[') 
  { 
    loopStart = i; 
    while(a[p]) interpret(buf[++i]); 
  } 
  if(c == ']') if(a[p]) i = loopStart; 
} 
 
void main(int argc, char *argv[]) 
{ 
  if(argv[1]) 
  { 
    FILE *fp = fopen(argv[1], "r"); 
    for(i=0;i<1024;i++) a[i]=0; 
    int j = 0; 
    while(j < 1024) buf[j++] = getc(fp); 
    for(i=0;i<1024;i++) if(buf[i]) interpret(buf[i]); 
  } 
  else 
    printf ( "File not specified. \ n Using: interp.exe"); 
 
  printf ( "\ n \ n Interpretation track. \ n"); 
  printf ( "Press [ENTER] to exit ...");  
  getchar(); 
} 
