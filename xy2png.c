/* x2xt2png.c - Chris X Edwards
First created 2019-02-13 for this blog post: http://xed.ch/b/2019/0213.html

This program simply inputs two numbers on lines from standard input and plots
them onto a PNG. That's it. Combined with Unix tricks it is very powerful and
because it is minimalistic C, it is very fast. So if you have some process
spewing out zillions of numbers, this is a reasonable way to plot them all.

Make sure you have these packages (probably):
    apt install libpng-dev libpng++-dev
Compile with something like this:
    gcc xy2png.c -o xy2png -lpng
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "png.h"
#define MAXLINELEN 666

void process_line(char *line, png_image i, png_bytep b) {
    const char *okchars= "0123456789.-+|, "; /* Optional: "eE" for sci notation. */
    for (int c= 0;c < strlen(line);c++) { if ( !strchr(okchars, line[c]) ) line[c]= ' '; }
    const char *delim= " |,"; char *x,*y;
    x= strtok(line, delim);
    if (x == NULL) { printf("Error: Could not parse X value.\n"); exit(1); }
    y= strtok(NULL, delim); /* When NULL, uses position in last searched string. */
    if (y == NULL) { printf("Error: Could not parse Y value.\n"); exit(1); }
    b[atoi(y) * i.width + atoi(x)]= 255;      /* Compute and set 1d position in the buffer. */
    /* if (verbose) printf("%d,%d\n",x,y); */
}

void process_file(FILE *fp, png_image i, png_bytep b){
    char *str= malloc(MAXLINELEN), *rbuf= str;
    int len= 0, bl= 0;
    if (str == NULL) {perror("Out of Memory!\n");exit(1);}
    while (fgets(rbuf,MAXLINELEN,fp)) {
        bl= strlen(rbuf); // Read buffer length.
        if (rbuf[bl-1] == '\n') { // End of buffer really is the EOL.
            process_line(str,i,b);
            free(str); // Clear and...
            str= malloc(MAXLINELEN); // ...reset this buffer.
            rbuf= str; // Reset read buffer to beginning.
            len= 0;
        } // End if EOL found.
        // Read buffer filled before line was completely input.
        else { // Add more mem and read some more of this line.
            len+= bl;
            str= realloc(str, len+MAXLINELEN); // Tack on some more memory.
            if (str == NULL) {perror("Out of Memory!\n");exit(1);}
            rbuf= str+len; // Slide the read buffer down to append position.
        } // End else add mem to this line.
    } // End while still bytes to be read from the file.
    fclose(fp);
    free(str);
}

int main(const int argc, char *argv[]) {
    png_image im; memset(&im, 0, sizeof im); /* Set up image structure and zero it out. */
    im.version= PNG_IMAGE_VERSION;           /* Encode what version of libpng made this. */
    im.height= 1000; im.width= 1000;         /* Image pixel dimensions. */
    png_bytep ibuf= malloc(PNG_IMAGE_SIZE(im)); /* Reserve image's memory buffer. */
    FILE *fp;                                /* Input file handle. */
    int optind= 0;
    if (argc == 1) {                         /* Use standard input if no files are specified. */
        fp= stdin;
        process_file(fp,im,ibuf);
    }
    else {
        while (optind<argc-1) { // Go through each file specified as an argument .
            optind++;
            if (*argv[optind] == '-') fp= stdin; // Dash as filename means use stdin here.
            else fp= fopen(argv[optind],"r");
            if (fp) process_file(fp,im,ibuf); // File pointer, fp, now correctly ascertained.
            else fprintf(stderr,"Could not open file:%s\n",argv[optind]);
        }
    }
    png_image_write_to_file (&im, "output.png", 0, ibuf, 0, NULL); /* PNG output. */
    return(EXIT_SUCCESS);
}
