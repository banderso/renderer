//
//  fileutil.c
//  Renderer
//
//  Created by Benjamin Anderson on 6/28/14.
//  Copyright (c) 2014 Benjamin Anderson. All rights reserved.
//

#include <stdio.h>

#include "fileutil.h"

char * BAR::ReadFile(const char * filename) {
    if (filename == NULL) return NULL;
    
    FILE * const file = fopen(filename, "r");
    if (file == NULL) return NULL;
    
    if (fseek(file, 0, SEEK_END)) {
        fprintf(stderr, "Failed to seek end of file.\n");
        return NULL;
    }
    
    long file_size = ftell(file);
    if (file_size == EOF) {
        perror("Error getting position");
        fclose(file);
        return NULL;
    }
    
    rewind(file);
    
    char *contents = new char[file_size + 1];
    if (contents == NULL) {
        fprintf(stderr, "Memory error.\n");
        return NULL;
    }
    
    size_t read_size = fread(contents, sizeof(char), file_size, file);
    contents[file_size] = '\0';
    
    if (read_size != file_size) {
        fprintf(stderr, "Error: file read length mismatch.\n");
    }
    
    fclose(file);
    
    return contents;
}

