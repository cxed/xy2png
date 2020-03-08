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
#include <getopt.h>
#include "png.h"
#define MAXLINELEN 666

int X=1000; int Y=1000; // Global image size defaults.

void usage() {
    printf(
    "Create PNG image of numerical X,Y input.\n"
    "Usage: xy2png [-h] [-x VAL] [-y VAL] [-f OUTPUT.PNG] [-|INPUTFILE...]\n"
    "Input Format: \"X Y\" (where \"0 0\" is left, top)\n" 
    "Options:\n"
    "  -h, --help                   display this help and exit\n"
    "  -x VAL, --width=VAL          width of output PNG [1000]\n"
    "  -y VAL, --height=VAL         height of output PNG [1000]\n"
    "  -o O.PNG, --out=O.PNG      output file to write PNG to [out.png]\n"
    );
}

void process_line(char *line, png_image i, png_bytep b) {
    char *ol= malloc(strlen(line) + 1); strcpy(ol,line); /* Save copy of original line. */
    const char *okchars= "0123456789.-+|, "; /* Optional: "eE" for sci notation. */
    for (int c= 0;c < strlen(line);c++) { if ( !strchr(okchars, line[c]) ) line[c]= ' '; }
    const char *delim= " |,"; char *x,*y;
    x= strtok(line, delim);
    if (x == NULL) { printf("Error: Could not parse X value: %s",ol); return; }
    int ix= atoi(x);
    if ((ix>X) | (ix<0)) { printf("Error: X value %s out of range.\n",x); return; }
    y= strtok(NULL, delim); /* When NULL, uses position in last searched string. */
    if (y == NULL) { printf("Error: Could not parse Y value: %s",ol); return; }
    int iy= atoi(y);
    if ((iy>Y) | (iy<0)) { printf("Error: Y value %s out of range.\n",y); return; }
    b[iy * i.width + ix]= 255;      /* Compute and set 1d position in the buffer. */
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
    char *ofn= "out.png";
    int o;
    while (1) {
        static struct option long_options[] = {
            {"help"  , no_argument,       NULL, 'h'}, /* Bools work well in C++. */
            {"width",  required_argument, NULL, 'x'}, /* Output width of PNG. */
            {"height", required_argument, NULL, 'y'}, /* Output height of PNG. */
            {"out",    required_argument, NULL, 'o'}, /* Filename of output PNG. */
            {0, 0, 0, 0} /* zero filled record to signal end */
        };
        o= getopt_long(argc, argv, "hx:y:o:", long_options, NULL);
        if (o == -1) break; /* Detect the end of the options. */
        switch (o) {
            case 'h': usage(); return(EXIT_SUCCESS); break;
            case 'x': if (optarg){ X= atoi(optarg); }; break;
            case 'y': if (optarg){ Y= atoi(optarg); }; break;
            case 'o': if (optarg){ ofn= optarg; }; break;
            default: printf("Unknown Option.\n"); return 0;
        } /* End options switch */
    } /* End options while loop */
    png_image im; memset(&im, 0, sizeof im); /* Set up image structure and zero it out. */
    im.version= PNG_IMAGE_VERSION;           /* Encode what version of libpng made this. */
    im.height= Y; im.width= X;               /* Image pixel dimensions. */
    png_bytep ibuf= malloc(PNG_IMAGE_SIZE(im)); /* Reserve image's memory buffer. */
    FILE *fp;                                /* Input file handle. */
    if (argc-optind == 0) {                  /* Use standard input if no files are specified. */
        printf("Standard Input...\n");
        fp= stdin;
        process_file(fp,im,ibuf);
    }
    else { /* Some files are specified. Maybe even a dash for stdin hiding amongst them. */
        while (optind<argc) { // Go through each file specified as an argument.
            /* printf("argc: %d optind: %d argv[optind]: %s\n",argc,optind,argv[optind]);*/
            if (*argv[optind] == '-') fp= stdin; // Dash as filename means use stdin here.
            else fp= fopen(argv[optind],"r");
            if (fp) process_file(fp,im,ibuf); // File pointer, fp, now correctly ascertained.
            else fprintf(stderr,"Could not open file:%s\n",argv[optind]);
            optind++;
        }
    }
    /* PNG output. */
    if (png_image_write_to_file (&im, ofn, 0, ibuf, 0, NULL)) {
        printf("Writing PNG: %s\n",ofn);
    }
    else {
      fprintf(stderr, "xy2png: write %s: %s\n", ofn, im.message);
    }
    return(EXIT_SUCCESS);
}
