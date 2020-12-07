/*
 * ICT1002 (C Language) Group Project.
 *
 * This file contains the definitions and function prototypes for all of
 * features of the ICT1002 chatbot.
 */

#ifndef _CHAT1002_H
#define _CHAT1002_H

#include <stdio.h>

/* the maximum number of characters we expect in a line of input (including the terminating null)  */
#define MAX_INPUT 256

/* the maximum number of characters allowed in the name of an intent (including the terminating null)  */
#define MAX_INTENT 32

/* the maximum number of characters allowed in the name of an entity (including the terminating null)  */
#define MAX_ENTITY 64

/* the maximum number of characters allowed in a response (including the terminating null) */
#define MAX_RESPONSE 256

/* the maximum number of intents allowed in a array (including the terminating null) */
#define MAX_NO_OF_INTENT 7

/* return codes for knowledge_get() and knowledge_put() */
#define KB_OK 0
#define KB_NOTFOUND -1
#define KB_INVALID -2
#define KB_NOMEM -3

/* functions defined in main.c */
int compare_token(const char *token1, const char *token2);
void prompt_user(char *buf, int n, const char *format, ...);

/* functions defined in chatbot.c */
const char *chatbot_botname();
const char *chatbot_username();
int chatbot_main(int inc, char *inv[], char *response, int n);
int chatbot_is_exit(const char *intent);
int chatbot_do_exit(int inc, char *inv[], char *response, int n);
int chatbot_is_load(const char *intent);
int chatbot_do_load(int inc, char *inv[], char *response, int n);
int chatbot_is_question(const char *intent);
int chatbot_do_question(int inc, char *inv[], char *response, int n);
int chatbot_is_reset(const char *intent);
int chatbot_do_reset(int inc, char *inv[], char *response, int n);
int chatbot_is_save(const char *intent);
int chatbot_do_save(int inc, char *inv[], char *response, int n);
int chatbot_is_smalltalk(const char *intent);
int chatbot_do_smalltalk(int inc, char *inv[], char *resonse, int n);

/* functions defined in knowledge.c */
int knowledge_get(const char *intent, const char *entity, char *response, int n);
int knowledge_put(const char *intent, const char *entity, const char *response);
void knowledge_reset();
int knowledge_read(FILE *f);
void knowledge_write(FILE *f);

typedef struct question
{
    char entity[MAX_ENTITY];
    char response[MAX_RESPONSE];
    struct question *next;

} QUESTION;

typedef struct intent
{
    char intent[MAX_INTENT];
    QUESTION *head_ptr;
    QUESTION *tail_ptr;
} INTENT;

typedef struct smalltalk
{
    char topic[MAX_ENTITY];
    char response[MAX_RESPONSE];

} SMALLTALK;

INTENT all_intents[MAX_NO_OF_INTENT];

/* functions defined in knowledge.c for utility purposes. */
void init_knowledge();
QUESTION *create_question(const char *entity, const char *response);
SMALLTALK *create_smalltalk(const char *topic, const char *response);
char *ltrim(char *s);
char *rtrim(char *s);
char *trim(char *s);
#endif