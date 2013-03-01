/* cwatch.h
 * Monitor file system activity using the inotify linux kernel library
 *
 * Copyright (C) 2012, Giuseppe Leone <joebew42@gmail.com>,
 *                     Vincenzo Di Cicco <enzodicicco@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 */
#ifndef __CWATCH_H
#define __CWATCH_H

#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <stdlib.h>
#include <syslog.h>
#include <errno.h>
#include <unistd.h>
#include <sys/inotify.h>
#include <sys/param.h>

#include "list.h"

/* Size of an event */
#define EVENT_SIZE      ( sizeof (struct inotify_event) )
#define EVENT_BUF_LEN   ( 1024 * ( EVENT_SIZE + 16 ) )

/*
 * _ROOT when cwatch execute the command will be replaced with the
 *       root monitored directory
 * _DIR  when cwatch execute the command will be replaced with the
 *       absolute full path of the directory where the event occured.
 * _TYPE when cwatch execute the command will be replaced with the
 *       event type occured
 */
#define COMMAND_PATTERN_ROOT   "{r}"
#define COMMAND_PATTERN_DIR    "{d}"
#define COMMAND_PATTERN_EVENT  "{e}"

/* Boolean data type */
typedef enum {FALSE,TRUE} bool_t;

/* Used to maintain information about watched resource */
typedef struct wd_data_s
{
    int wd;               /* watch descriptor */
    char *path;           /* absoulete real path of the directory */
    bool_t symbolic_link; /* used to know if is reached by symbolic link */
    LIST *links;          /* list of sym links that point to this resource */
} WD_DATA;

/* Used by str_split (see function definition below) */
typedef struct str_split_s
{
    unsigned short size;
    char **substring;
} STR_SPLIT_S;

/* Environment and variables */
char *program_name;
char *program_version;
char *path;
char *command;
STR_SPLIT_S *scommand;
int fd;
LIST *list_wd;

bool_t be_syslog;
bool_t be_verbose;
bool_t be_easter;

/**
 * Print the version of the program and exit
 */
void print_version();

/**
 * Help
 * 
 * Print out the help
 */
void help();

/**
 * Log
 * 
 * Log message via syslog or via standard output
 * @param char * : Message to log
 */
void log_message(char *);

/**
 * Print List
 * 
 * Only an help fuction that print list_wd
 * @param LIST * : the list_wd you want print
 */
void print_list(LIST *);

/**
 * Resolve the real path
 * 
 * This function is used to resolve the
 * absolute real_path of a symbolic link or a relative path.
 * @param char *  : the path of the symbolic link to resolve
 * @return char * : the resolved real_path, NULL otherwise
 */
char *resolve_real_path(const char *);

/**
 * Searchs and returns the node for the path passed
 * as argument.
 * @param char *       : The path to find
 * @return LIST_NODE * : A pointer to node, NULL otherwise.
 */
LIST_NODE *get_from_path(char *);

/**
 * Searchs and returns the node for the wd passed
 * as argument.
 * @param int          : The wd to find
 * @return LIST_NODE * : a pointer to node, NULL otherwise.
 */
LIST_NODE *get_from_wd(int);

/**
 * Parse command line
 *
 * This function is used to parse command line and initialize some environment variables
 * @param int     : arguments count
 * @param char ** : arguments value
 * @return int
 */
int parse_command_line(int, char **);

/**
 * Watch a directory
 *
 * It performs a breath-first-search to traverse a directory and
 * call the add_to_watch_list(path) for each directory, either if it's pointed by a symbolic link or not.
 * @param char * : The path of directory to watch
 * @param char * : The symbolic link that point to the path
 * @return int   : -1 (An error occurred), 0 (Resource added correctly)
 */
int watch(char *, char *);

/**
 * Add a directory into watch list
 *
 * This function is used to append a directory into watch list
 * @param char* : The absolute path of the directory to watch
 * @param char* : The symbolic link that point to the path
 * @return LIST_NODE* : the pointer of the node of the watch list
 */
LIST_NODE *add_to_watch_list(char *, char *);

/**
 * Checks whetever a string exists in a list
 *
 * @param char * : string to check
 * @param LIST * : list containing string
 */
int exists(char *, LIST *);

/**
 * Unwatch a directory
 * 
 * Used to remove a file or directory
 * from the list of watched resources
 * @param char * : the path of the resource to remove
 * @param bool_t : TRUE if the path to unwatch is a symlink, FALSE otherwise.
 */
void unwatch(char *, bool_t);

/**
 * Start monitoring
 * 
 * Used to monitor inotify event on watched resources
 */
int monitor();

/**
 * str_split
 *
 * subdivide a string into n-substring, that are separate
 * by a specified separator symbol.
 * @param char *      : the string to subdivide
 * @param char *      : separator symbol
 * @param STR_SPLIT_R : a data structure contains the splitted string
 */
STR_SPLIT_S *str_split(char *, char *);

/**
 * Execute a command
 *
 * This function handle the execution of a command 
 * @param char * : the inotify event (maybe is not a char * ...)
 * @param char * : the path where event occured
 * @return int:  : -1 in case of error, 0 otherwise
 */
int execute_command(char *, char *);

#endif /* !__CWATCH_H */
