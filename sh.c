#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <limits.h>
#include <unistd.h>
#include <stdlib.h>
#include <pwd.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include "sh.h"

int sh( int argc, char **argv, char **envp) {
  char *prompt = calloc(PROMPTMAX, sizeof(char));
  char *commandline = calloc(MAX_CANON, sizeof(char));
  char *command, *arg, *commandpath, *p, *pwd, *owd;
  char **args = NULL;// = calloc(MAXARGS, sizeof(char*));
  int uid, i, status, argsct, go = 1;
  struct passwd *password_entry;
  char *homedir;
  struct pathelement *pathlist;

  uid = getuid();
  password_entry = getpwuid(uid);               /* get passwd info */
  homedir = password_entry->pw_dir;		/* Home directory to start
						  out with*/
     
  if ((pwd = getcwd(NULL, PATH_MAX+1)) == NULL) {
    perror("getcwd");
    exit(2);
  }
  owd = calloc(strlen(pwd) + 1, sizeof(char));
  memcpy(owd, pwd, strlen(pwd));
  free(pwd);
  prompt[0] = '?'; prompt[1] = ' '; prompt[2] = '\0';
  printf("Type \"help\" for a list of commands.\n");
  /* Put PATH into a linked list */
  pathlist = get_path();
  int contin = 1;
  while (contin) {
    /* print your prompt */
    printf("%s", prompt);
    /* get command line and process */
    fgets(commandline, MAX_CANON, stdin);
    freeStringArray(args);
    args = stringToArray(commandline);
    printf("%s\n", args[0]);
     //contin = 0;
    /* check for each built in command and implement */
    if (strncmp(args[0], "exit", 4) == 0) {
      contin = 0;
    } else if (strncmp(args[0], "help", 4) == 0) {
      printf("exit: exit the shell\n");
      printf("help: display list of commands\n");
      printf("which: find address of executable file\n");
      printf("prompt (newprompt): change prompt to newprompt\n");
    } else if (strncmp(args[0], "which", 5) == 0) {
      char* paff = which(args[1], pathlist);
      //printf("%s\n", paff);
      free(paff);
    } else if (strncmp(args[0], "prompt", 6) == 0) {
      if (args[1] != NULL) {
        strcpy(prompt, args[1]);
        prompt[strlen(prompt)-1] = ' ';
        prompt[strlen(prompt)] = '\0';
      } //this might be a problem if you put in a prompt of max length, but I'm sure that's never going to happen
    } else {/*  else  program to exec */
       /* find it */
      char* paff = which(args[0], pathlist);
      //printf("%s\n", paff); //when i uncomment this it segfaults
      if (paff == NULL) {
        printf("Command could not be found\n");
      } else {
        printf("Program could be found and can be executed, but I'm not dealing with that just yet\n");
      }
      free(paff);
       /* do fork(), execve() and waitpid() */

      /* else */
        /* fprintf(stderr, "%s: Command not found.\n", args[0]); */
    }
  }
  freeStringArray(args);
  free(prompt);
  free(commandline);
  free(owd);
  return 0;
} /* sh() */

/**
  Allocates and returns memory only on success
*/
char *which(char *command, struct pathelement *pathlist) {
   /* loop through pathlist until finding command and return it.  Return
   NULL when not found. */
  printf("%s/n", command);
  while (pathlist != NULL) {
    printf("%s\n", pathlist->element);
    int len = sizeof(char) * (strlen(command) + strlen(pathlist->element) + 2);
    char* paff = malloc(len);
    
    strcpy(paff, pathlist->element);
    strcat(paff, "/");
    strcat(paff, command);
    paff[len-1] = '\0';
    printf("%s\n", paff);
    int acc = access(paff, X_OK);
    //printf("%d\n", acc);
    if (acc >= 0) {
      return paff;
    }
    free(paff);
    pathlist = pathlist->next;
  }
  return NULL;
}

char *where(char *command, struct pathelement *pathlist) {
  /* similarly loop through finding all locations of command */
  return which(command, pathlist);
}

void list (char *dir) {
  /* see man page for opendir() and readdir() and print out filenames for
  the directory passed */
} /* list() */

/**
  Allocates memory.
*/
char** stringToArray(char* input) {
  char buf[MAX_CANON];
  strcpy(buf, input);
  char* t = strtok(buf, " ");
  int cnt = 1;
  while(strtok(NULL, " "))
    cnt++;
  char** argv = malloc((cnt+1) * sizeof(char*));
  argv[cnt] = NULL;
  
  cnt = 0;
  strcpy(buf, input);
  t = strtok(buf, " ");
  while (t) {
    int len = strlen(t);
    argv[cnt] = (char*) malloc((len+1)*sizeof(char));
    strcpy(argv[cnt], t);
    cnt++;
    t = strtok(NULL, " ");
  }
  return argv;
}

/**
  frees an array of strings
*/
int freeStringArray(char** ray) {
  if (ray == NULL)
    return 0;
  for (int i = 0; ray[i] != NULL; i++)
    free(ray[i]);
  free(ray);
  return 1;
}
