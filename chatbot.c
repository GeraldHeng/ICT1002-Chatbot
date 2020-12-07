/*
 * ICT1002 (C Language) Group Project.
 *
 * This file implements the behaviour of the chatbot. The main entry point to
 * this module is the chatbot_main() function, which identifies the intent
 * using the chatbot_is_*() functions then invokes the matching chatbot_do_*()
 * function to carry out the intent.
 *
 * chatbot_main() and chatbot_do_*() have the same method signature, which
 * works as described here.
 *
 * Input parameters:
 *   inc      - the number of words in the question
 *   inv      - an array of pointers to each word in the question
 *   response - a buffer to receive the response
 *   n        - the size of the response buffer
 *
 * The first word indicates the intent. If the intent is not recognised, the
 * chatbot should respond with "I do not understand [intent]." or similar, and
 * ignore the rest of the input.
 *
 * If the second word may be a part of speech that makes sense for the intent.
 *    - for WHAT, WHERE and WHO, it may be "is" or "are".
 *    - for SAVE, it may be "as" or "to".
 *    - for LOAD, it may be "from".
 * The word is otherwise ignored and may be omitted.
 *
 * The remainder of the input (including the second word, if it is not one of the
 * above) is the entity.
 *
 * The chatbot's answer should be stored in the output buffer, and be no longer
 * than n characters long (you can use snprintf() to do this). The contents of
 * this buffer will be printed by the main loop.
 *
 * The behaviour of the other functions is described individually in a comment
 * immediately before the function declaration.
 *
 * You can rename the chatbot and the user by changing chatbot_botname() and
 * chatbot_username(), respectively. The main loop will print the strings
 * returned by these functions at the start of each line.
 */

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include "chat1002.h"

/*
 * Get the name of the chatbot.
 *
 * Returns: the name of the chatbot as a null-terminated string
 */
const char *chatbot_botname()
{

	return "Chatbot";
}

/*
 * Get the name of the user.
 *
 * Returns: the name of the user as a null-terminated string
 */
const char *chatbot_username()
{

	return "User";
}

/*
 * Get a response to user input.
 *
 * See the comment at the top of the file for a description of how this
 * function is used.
 *
 * Returns:
 *   0, if the chatbot should continue chatting
 *   1, if the chatbot should stop (i.e. it detected the EXIT intent)
 */
int chatbot_main(int inc, char *inv[], char *response, int n)
{
	if (strlen(all_intents[0].intent) == 0)
	{
		init_knowledge();
	}

	/* check for empty input */
	if (inc < 1)
	{
		snprintf(response, n, "");
		return 0;
	}

	/* look for an intent and invoke the corresponding do_* function */
	if (chatbot_is_exit(inv[0]))
		return chatbot_do_exit(inc, inv, response, n);
	else if (chatbot_is_load(inv[0]))
		return chatbot_do_load(inc, inv, response, n);
	else if (chatbot_is_question(inv[0]))
		return chatbot_do_question(inc, inv, response, n);
	else if (chatbot_is_reset(inv[0]))
		return chatbot_do_reset(inc, inv, response, n);
	else if (chatbot_is_save(inv[0]))
		return chatbot_do_save(inc, inv, response, n);
	else if (chatbot_is_smalltalk(inv[0]))
		return chatbot_do_smalltalk(inc, inv, response, n);
	else
	{
		snprintf(response, n, "I don't understand \"%s\".", inv[0]);
		return 0;
	}
}

/*
 * Determine whether an intent is EXIT.
 *
 * Input:
 *  intent - the intent
 *
 * Returns:
 *  1, if the intent is "exit" or "quit"
 *  0, otherwise
 */
int chatbot_is_exit(const char *intent)
{
	return compare_token(intent, "exit") == 0 ||
		   compare_token(intent, "quit") == 0;
}

/*
 * Perform the EXIT intent.
 *
 * See the comment at the top of the file for a description of how this
 * function is used.
 *
 * Returns:
 *   0 (the chatbot always continues chatting after a question)
 */
int chatbot_do_exit(int inc, char *inv[], char *response, int n)
{
	int quit = chatbot_do_reset(inc, inv, response, n);
	snprintf(response, n, "Goodbye!");

	return 1;
}

/*
 * Determine whether an intent is LOAD.
 *
 * Input:
 *  intent - the intent
 *
 * Returns:
 *  1, if the intent is "load"
 *  0, otherwise
 */
int chatbot_is_load(const char *intent)
{
	return compare_token(intent, "load") == 0;
}

/*
 * Load a chatbot's knowledge base from a file.
 *
 * See the comment at the top of the file for a description of how this
 * function is used.
 *
 * Returns:
 *   0 (the chatbot always continues chatting after loading knowledge)
 */
int chatbot_do_load(int inc, char *inv[], char *response, int n)
{
	char *filename = NULL, *word;
	FILE *file;

	// Check if there is a word contain .ini inside inv[], if there is,
	// that is the filename.
	for (int i = 1; i < inc; i++)
	{
		word = inv[i];
		if (strstr(word, ".ini"))
		{
			filename = word;
			break;
		}
	}

	// If filename is not found in inv, return no file detected.
	if (filename == NULL)
	{
		snprintf(response, n, "No file path detected.");
		return 0;
	}

	// Open file on read mode. Make sure file is opened and is not NULL.
	file = fopen(filename, "r");
	if (file == NULL)
	{
		snprintf(response, n, "%s not found.", filename);
	}
	else
	{
		// Read the open file and get back number of lines read.
		int lines_read = knowledge_read(file);
		snprintf(response, n, "Successfully loaded %d responses from %s",
				 lines_read, filename);
	}

	return 0;
}

/*
 * Determine whether an intent is a question.
 *
 * Input:
 *  intent - the intent
 *
 * Returns:
 *  1, if the intent is "what", "where", or "who"
 *  0, otherwise
 */
int chatbot_is_question(const char *intent)
{
	for (int i = 0; i < MAX_NO_OF_INTENT - 1; i++)
	{
		if (compare_token(intent, all_intents[i].intent) == 0)
		{
			return 1;
		}
	}
	return 0;
}

/*
 * Answer a question.
 *
 * inv[0] contains the the question word.
 * inv[1] may contain "is" or "are"; if so, it is skipped.
 * The remainder of the words form the entity.
 *
 * See the comment at the top of the file for a description of how this
 * function is used.
 *
 * Returns:
 *   0 (the chatbot always continues chatting after a question)
 */
int chatbot_do_question(int inc, char *inv[], char *response, int n)
{
	if (inc < 3)
	{
		snprintf(response, n, "Invalid question!");
		return 0;
	}

	char *intent = inv[0];

	// Create pointer to point to entity. Must be not NULL.
	char *entity = (char *)malloc(MAX_ENTITY);
	if (entity == NULL)
	{
		return KB_NOMEM;
	}
	strcpy(entity, "");

	// Create pointer to point to whole_question. Must be not NULL.
	char *whole_question = (char *)malloc(MAX_INPUT);
	if (whole_question == NULL)
	{
		return KB_NOMEM;
	}
	strcpy(whole_question, "I don't know. ");

	// Derive entity and whole_question from inv[] here.
	int i = 0;
	while (1)
	{
		// Check for the end of entity which will be NULL.
		if (i > 1)
		{
			if (inv[i] == NULL)
			{
				entity[strlen(entity) - 1] = '\0';
				whole_question[strlen(whole_question) - 1] = '\0';
				break;
			}

			// If current inv[i] is not NULL, add it to entity string.
			strcat(entity, inv[i]);
			strcat(entity, " ");
		}
		strcat(whole_question, inv[i]);
		strcat(whole_question, " ");
		i++;
	}

	// Try to get response from knowledge and return into response buffer.
	int status = knowledge_get(intent, entity, response, n);

	// If entity is not found, as user to input response for new entity.
	if (status == KB_NOTFOUND)
	{
		char user_input[MAX_INPUT];
		strcat(whole_question, "?");
		prompt_user(user_input, MAX_INPUT, whole_question);

		// Display :-( if user input is empty.
		if (compare_token(user_input, "") == 0)
		{
			snprintf(response, n, ":-(");
		}
		else
		{
			// Put new question into knowledge of chatbot.Î
			status = knowledge_put(intent, entity, user_input);
			if (status == KB_OK)
			{
				snprintf(response, n, "Thank You.");
			}
			else
			{
				snprintf(response, n, "Something went wrong!");
			}
		}
	}
	// Entity is found, display the respective response.
	else if (status == KB_OK)
	{
		snprintf(response, n, "%s", response);
	}
	else if (status == KB_NOMEM)
		snprintf(response, n, "No memory currently!");
	else
		snprintf(response, n, "Something when wrong!");

	// Free pointers used.
	free(entity);
	free(whole_question);
	return 0;
}

/*
 * Determine whether an intent is RESET.
 *
 * Input:
 *  intent - the intent
 *
 * Returns:
 *  1, if the intent is "reset"
 *  0, otherwise
 */
int chatbot_is_reset(const char *intent)
{
	return compare_token(intent, "reset") == 0;
}

/*
 * Reset the chatbot.
 *
 * See the comment at the top of the file for a description of how this
 * function is used.
 *
 * Returns:
 *   0 (the chatbot always continues chatting after beign reset)
 */
int chatbot_do_reset(int inc, char *inv[], char *response, int n)
{
	// Reset knowledge.
	if (all_intents[0].head_ptr == NULL && all_intents[1].head_ptr == NULL && all_intents[2].head_ptr == NULL && all_intents[3].head_ptr == NULL && all_intents[4].head_ptr == NULL && all_intents[5].head_ptr == NULL)
	{
		snprintf(response, n, "Nothing to reset.");
		return 0;
	}
	knowledge_reset();
	snprintf(response, n, "Chatbot Reset.");
	return 0;
}

/*
 * Determine whether an intent is SAVE.
 *
 * Input:
 *  intent - the intent
 *
 * Returns:
 *  1, if the intent is "what", "where", or "who"
 *  0, otherwise
 */
int chatbot_is_save(const char *intent)
{
	return compare_token(intent, "save") == 0;
}

/*
 * Save the chatbot's knowledge to a file.
 *
 * See the comment at the top of the file for a description of how this
 * function is used.
 *
 * Returns:
 *   0 (the chatbot always continues chatting after saving knowledge)
 */
int chatbot_do_save(int inc, char *inv[], char *response, int n)
{
	char *filename = NULL, *word;
	FILE *file;

	// Check if there is a word contain .ini inside inv[], if there is,
	// that is the filename.
	for (int i = 1; i < inc; i++)
	{
		word = inv[i];
		if (strstr(word, ".ini"))
		{
			filename = word;
			break;
		}
	}

	// Open user file in write mode and write current knowledge into user file.
	file = fopen(filename, "w");
	if (file == NULL)
	{
		snprintf(response, n, "Error when opening file!");
		return 0;
	}

	// Write into file.
	knowledge_write(file);
	snprintf(response, n, "My knowledge has been saved to %s.", filename);
	fclose(file);

	return 0;
}

/*
 * Determine which an intent is smalltalk.
 *
 *
 * Input:
 *  intent - the intent
 *
 * Returns:
 *  1, if the intent is the first word of one of the smalltalk phrases
 *  0, otherwise
 */
int chatbot_is_smalltalk(const char *intent)
{
	for (int i = 0; i < MAX_NO_OF_INTENT - 1; i++)
	{
		if (compare_token(intent, all_intents[i].intent) == 0)
		{
			return 0;
		}
	}

	return 1;
}

/*
 * Respond to smalltalk.
 *
 * See the comment at the top of the file for a description of how this
 * function is used.
 *
 * Returns:
 *   0, if the chatbot should continue chatting
 *   1, if the chatbot should stop chatting (e.g. the smalltalk was "goodbye" etc.)
 */
int chatbot_do_smalltalk(int inc, char *inv[], char *response, int n)
{
	// All defined smalltalks topic and response.
	SMALLTALK *smalltalk_ptrs[7] = {
		create_smalltalk("hello", "Greetings."),
		create_smalltalk("how", "An interesting question. I never really thought about it."),
		create_smalltalk("weather", "Both good and bad weather should always be appreciated."),
		create_smalltalk("life", "Life always has it's ups and downs."),
		create_smalltalk("hot", "Know what else is hot? You."),
		create_smalltalk("purpose", "An interesting question. Currently, I am here for your personal needs but maybe I will mean more to someone else ;-;")};

	// Bot response to user.
	char *bot_response = (char *)malloc(MAX_RESPONSE);
	if (bot_response == NULL)
	{
		snprintf(response, n, "No memory currently!");
		exit(0);
	}
	strcpy(bot_response, "");

	// Check of response for user input is found.
	int response_found = 0;

	// Loop through all the word of input of user.
	for (int x = 0; x < inc; x++)
	{
		int i = 0;
		// Loop through all the smalltalks until a common topic is found.
		while (1)
		{
			if (smalltalk_ptrs[i] == NULL)
			{
				break;
			}
			// If topic is found, concat the response into bot_response.
			if (compare_token(inv[x], smalltalk_ptrs[i]->topic) == 0)
			{
				strcat(bot_response, smalltalk_ptrs[i]->response);
				strcat(bot_response, " ");
				response_found = 1;
			}
			i++;
		}
	}

	// If no response is found, Add this to bot_response.
	if (response_found == 0)
	{
		strcat(bot_response, "I see.");
	}

	snprintf(response, n, "%s", bot_response);
	// Free pointers used.
	free(bot_response);
	return 0;
}
